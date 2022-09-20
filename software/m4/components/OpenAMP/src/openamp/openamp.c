/**
  ******************************************************************************
  * @file   openamp.c
  * @author  MCD Application Team
  * @brief  Code for openamp applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "openamp.h"
#include "rsc_table.h"
#include "metal/sys.h"
#include "metal/device.h"


#define SHM_DEVICE_NAME         "STM32_SHM"


static struct metal_io_region *shm_io;
static struct metal_io_region *rsc_io;
static struct shared_resource_table *rsc_table;
static struct rpmsg_virtio_shm_pool shpool;
static struct rpmsg_virtio_device rvdev;


static metal_phys_addr_t shm_physmap;

struct metal_device shm_device = {
  .name = SHM_DEVICE_NAME,
  .num_regions = 2,
  .regions = {
      {.virt = NULL}, /* shared memory */
      {.virt = NULL}, /* rsc_table memory */
  },
  .node = { NULL },
  .irq_num = 0,
  .irq_info = NULL
};


static int OPENAMP_shmem_init(int RPMsgRole)
{
  int status = 0;
  struct metal_device *device = NULL;

  /*
    We use the default parameters to initialize the libmetal API.
  */
  struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
  void* rsc_tab_addr = NULL;
  int rsc_size = 0;

  /*
    Init the libmetal API.
  */
  metal_init(&metal_params);

  /*
    Register the Shared memory on the generic bus.
  */
  status = metal_register_generic_device(&shm_device);
  if (status != 0) {
    return status;
  }

  /*
    Open the registered Shared Memory Device.
  */
  status = metal_device_open("generic", SHM_DEVICE_NAME, &device);
  if (status != 0) {
    return status;
  }

  shm_physmap = SHM_START_ADDRESS;

  /*
    Initialize the Shared Memory Block.
  */
  metal_io_init(&device->regions[0], (void *)SHM_START_ADDRESS, &shm_physmap,
                SHM_SIZE, (unsigned int)-1, 0, NULL);

  shm_io = metal_device_io_region(device, 0);
  if (shm_io == NULL) {
    return -1;
  }

  /*
    On the M4, this only gets us the resource_table pointer and the size of the resource table.
  */
  resource_table_init(RPMsgRole, &rsc_tab_addr, &rsc_size);
  rsc_table = (struct shared_resource_table *)rsc_tab_addr;
  if (!rsc_table)
  {
    return -1;
  }

  /*
    Initialize the resource_table as the second io region of the Shared Memory Device.
  */
  metal_io_init(&device->regions[1], rsc_table,
               (metal_phys_addr_t *)rsc_table, rsc_size, -1U, 0, NULL);

  rsc_io = metal_device_io_region(device, 1);
  if (rsc_io == NULL) {
    return -1;
  }

  return 0;
}

int MX_OPENAMP_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb)
{
  struct fw_rsc_vdev_vring *vring_rsc = NULL;
  struct virtio_device *vdev = NULL;
  int status = 0;


  MAILBOX_Init();

  /*
    This function initializes the libmetal API and registers a "Shared Memory" Device on the generic
    device bus. This device has 2 IO regions (the memory and the resource table) that also get initialized.
  */
  status = OPENAMP_shmem_init(RPMsgRole);
  if(status)
  {
    return status;
  }


  /*
    Create a virtio device.
  */
  vdev = rproc_virtio_create_vdev(RPMsgRole, VDEV_ID, &rsc_table->vdev,
                                  rsc_io, NULL, MAILBOX_Notify, NULL);
  if (vdev == NULL)
  {
    return -1;
  }


  /*
    Wait blocking for the Host (Linux in this case).
  */
  rproc_virtio_wait_remote_ready(vdev);

  vring_rsc = &rsc_table->vring0;
  status = rproc_virtio_init_vring(vdev, 0, vring_rsc->notifyid,
                                   (void *)vring_rsc->da, shm_io,
                                   vring_rsc->num, vring_rsc->align);
  if (status != 0)
  {
    return status;
  }


  vring_rsc = &rsc_table->vring1;
  status = rproc_virtio_init_vring(vdev, 1, vring_rsc->notifyid,
                                   (void *)vring_rsc->da, shm_io,
                                   vring_rsc->num, vring_rsc->align);
  if (status != 0)
  {
    return status;
  }


  rpmsg_virtio_init_shm_pool(&shpool, (void *)VRING_BUFF_ADDRESS,
                             (size_t)SHM_SIZE);
  rpmsg_init_vdev(&rvdev, vdev, ns_bind_cb, shm_io, &shpool);


  return 0;
}

void OPENAMP_DeInit()
{


  rpmsg_deinit_vdev(&rvdev);

  metal_finish();

}

int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb)
{
  int ret = 0;

  ret = rpmsg_create_ept(ept, &rvdev.rdev, name, RPMSG_ADDR_ANY, dest, cb,
		          unbind_cb);

  return ret;
}

void OPENAMP_check_for_message(void)
{

  MAILBOX_Poll(rvdev.vdev);
}

void OPENAMP_Wait_EndPointready(struct rpmsg_endpoint *rp_ept)
{
  /* USER CODE BEGIN EP_READY */

  /* USER CODE END EP_READY */

  while(!is_rpmsg_ept_ready(rp_ept))
  {
    /* USER CODE BEGIN 0 */

    /* USER CODE END 0 */
      MAILBOX_Poll(rvdev.vdev);

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */
  }
}

