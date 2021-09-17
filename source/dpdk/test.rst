UIO Init
========

I will take **vmbus** bus and **uio_hv_generic** driver as example if need to show the codes.
It's just because I setup DPDK environment in WSL2 in which vm the NIC uses this type of bus
and driver.

In the DPDK, some registry happen before **main** function, it uses the *__attribute__(constructored(prio))*
with four levels of priority to achieve it.

.. code::

    #define RTE_PRIORITY_LOG 101
    #define RTE_PRIORITY_BUS 110
    #define RTE_PRIORITY_CLASS 120
    #define RTE_PRIORITY_LAST 65535

Register bus
------------

Each bus owns common callbacks to *scan* or *probe* the devices and its *driver* list 
and *device* list to manage the drivers and devices belong to this bus.

*rte_vmbus_bus* is used by **vm_bus**.

.. code::

    struct rte_vmbus_bus rte_vmbus_bus = {
        .bus = {
            .scan = rte_vmbus_scan,
            .probe = rte_vmbus_probe,
            .find_device = vmbus_find_device,
            .parse = vmbus_parse,
        },
        .device_list = TAILQ_HEAD_INITIALIZER(rte_vmbus_bus.device_list),
        .driver_list = TAILQ_HEAD_INITIALIZER(rte_vmbus_bus.driver_list),
    };

    RTE_REGISTER_BUS(vmbus, rte_vmbus_bus.bus);

**RTE_REGISTER_BUS** Marco is an encapsulation of register api

.. code:: 

    #define RTE_REGISTER_BUS(nm, bus) \
    RTE_INIT_PRIO(businitfn_ ##nm, BUS) \
    {\
        (bus).name = RTE_STR(nm);\
        rte_bus_register(&bus); \
    }

after registry, the *rte_vmbus_bus* will be inserted into *rte_bus_list*.

.. code::

    void rte_bus_register(struct rte_bus *bus)
    {
        ...
        TAILQ_INSERT_TAIL(&rte_bus_list, bus, next);
        RTE_LOG(DEBUG, EAL, "Registered [%s] bus.\n", bus->name);
    }

Register PMD Driver
-------------------

A Poll Mode Driver (PMD) consists of APIs, provided through the BSD driver 
running in user space, to configure the devices and their respective queues. 
In addition, a PMD accesses the RX and TX descriptors directly without any 
interrupts (with the exception of Link Status Change interrupts) to quickly 
receive, process and deliver packets in the user’s application. This section 
describes the requirements of the PMDs, their global design principles and 
proposes a high-level architecture and a generic external API for the Ethernet PMDs.

*rte_netvsc_pmd* is the **hyper-v** type PMD

.. code::

    static struct rte_vmbus_driver rte_netvsc_pmd = {
        .id_table = hn_net_ids,
        .probe = eth_hn_probe,
        .remove = eth_hn_remove,
    }

There is another marco to register PMD driver to **vm_bus**.

..  code::

    RTE_PMD_REGISTER_VMBUS(net_netvsc, rte_netvsc_pmd);

    #define RTE_PMD_REGISTER_VMBUS(nm, vmbus_drv)		\
        RTE_INIT(vmbusinitfn_ ##nm)			\
        {						\
            (vmbus_drv).driver.name = RTE_STR(nm);	\
            rte_vmbus_register(&vmbus_drv);		\
        }						\
        RTE_PMD_EXPORT_NAME(nm, __COUNTER__)

**rte_vmbus_register** will insert *rte_netvsc_pmd* to the list of **vm_bus**

.. code::

    /* register vmbus driver */
    void rte_vmbus_register(struct rte_vmbus_driver *driver)
    {
        TAILQ_INSERT_TAIL(&rte_vmbus_bus.driver_list, driver, next);
        driver->bus = &rte_vmbus_bus;
    }


Register Device
---------------

In eal initialization, it will scan all the devices