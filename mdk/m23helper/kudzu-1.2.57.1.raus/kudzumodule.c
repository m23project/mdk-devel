/* Copyright 2000-2003 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "Python.h"
#include "kudzu.h"
#include "device.h"
#include "adb.h"
#include "ddc.h"
#include "firewire.h"
#include "ide.h"
#include "isapnp.h"
#include "keyboard.h"
#include "macio.h"
#include "parallel.h"
#include "pci.h"
#include "pcmcia.h"
#include "psaux.h"
#include "s390.h"
#include "sbus.h"
#include "scsi.h"
#include "serial.h"
#include "usb.h"
#include "vio.h"
#include "xen.h"

typedef struct {
    char * name;
    int value;
} TableEntry;

static TableEntry classTable[] = {
    { "CLASS_UNSPEC",	CLASS_UNSPEC },
    { "CLASS_OTHER",	CLASS_OTHER },
    { "CLASS_NETWORK",	CLASS_NETWORK },
    { "CLASS_SCSI",	CLASS_SCSI },
    { "CLASS_MOUSE",	CLASS_MOUSE },    
    { "CLASS_AUDIO",	CLASS_AUDIO },
    { "CLASS_CDROM",	CLASS_CDROM },    
    { "CLASS_MODEM",	CLASS_MODEM },    
    { "CLASS_VIDEO",	CLASS_VIDEO },
    { "CLASS_TAPE",	CLASS_TAPE },    
    { "CLASS_FLOPPY",	CLASS_FLOPPY },    
    { "CLASS_SCANNER",	CLASS_SCANNER },    
    { "CLASS_HD",	CLASS_HD },    
    { "CLASS_RAID",	CLASS_RAID },    
    { "CLASS_PRINTER",	CLASS_PRINTER },
    { "CLASS_CAPTURE",	CLASS_CAPTURE },
    { "CLASS_KEYBOARD",	CLASS_KEYBOARD },
    { "CLASS_SOCKET",	CLASS_SOCKET },
    { "CLASS_MONITOR",	CLASS_MONITOR },
    { "CLASS_USB", CLASS_USB },
    { "CLASS_FIREWIRE", CLASS_FIREWIRE },
    { "CLASS_IDE", CLASS_IDE },
    { "CLASS_ATA", CLASS_ATA },
    { "CLASS_SATA", CLASS_SATA },
    { NULL },
} ;

static int numClassEntries = sizeof (classTable) / sizeof (TableEntry);

static TableEntry busTable[] = {
    { "BUS_UNSPEC",	BUS_UNSPEC },
    { "BUS_OTHER",	BUS_OTHER },
    { "BUS_PCI",	BUS_PCI },
    { "BUS_SBUS",	BUS_SBUS },
    { "BUS_SERIAL",	BUS_SERIAL },
    { "BUS_PSAUX",	BUS_PSAUX },
    { "BUS_PARALLEL",	BUS_PARALLEL },
    { "BUS_SCSI",	BUS_SCSI },
    { "BUS_IDE",	BUS_IDE },
    { "BUS_KEYBOARD",	BUS_KEYBOARD },
    { "BUS_DDC",	BUS_DDC },
    { "BUS_USB",	BUS_USB },
    { "BUS_ISAPNP",	BUS_ISAPNP },
    { "BUS_MISC",	BUS_MISC },
    { "BUS_FIREWIRE",	BUS_FIREWIRE},
    { "BUS_PCMCIA",	BUS_PCMCIA},
    { "BUS_ADB",	BUS_ADB},
    { "BUS_MACIO",	BUS_MACIO},
    { "BUS_VIO",        BUS_VIO},
    { "BUS_S390",       BUS_S390},
    { "BUS_XEN",        BUS_XEN},
    { NULL }
};

static int numBusEntries = sizeof (busTable) / sizeof (TableEntry);

static TableEntry modeTable[] = {
    { "PROBE_ALL",	PROBE_ALL },
    { "PROBE_SAFE",	PROBE_SAFE },
    { "PROBE_ONE",	PROBE_ONE },
    { NULL },
} ;
static int numModeEntries = sizeof (modeTable) / sizeof (TableEntry);


void addDDCInfo(PyObject *dict,struct ddcDevice * device)
{
    PyObject *o;

    if(device->id) {
        PyDict_SetItemString(dict,"id",
			     o=PyString_FromString(device->id));
	Py_DECREF(o);
    } else {
		PyDict_SetItemString(dict,"id",Py_None); 
    }
    PyDict_SetItemString(dict,"horizSyncMin",
			 o=PyInt_FromLong(device->horizSyncMin));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"horizSyncMax",
			 o=PyInt_FromLong(device->horizSyncMax));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"vertRefreshMin",
			 o=PyInt_FromLong(device->vertRefreshMin));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"vertRefreshMax",
			 o=PyInt_FromLong(device->vertRefreshMax));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"mem",o=PyInt_FromLong(device->mem));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"physicalWidth",o=PyInt_FromLong(device->physicalWidth));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"physicalHeight",o=PyInt_FromLong(device->physicalHeight));
    Py_DECREF(o);
}

void addIDEInfo(PyObject *dict,struct ideDevice * device)
{
    PyObject *o;

    if (device->physical) {
	    PyDict_SetItemString(dict,"physical",
				 o=PyString_FromString(device->physical));
	    Py_DECREF(o);
	    
    } else
	    PyDict_SetItemString(dict,"physical", Py_None);
    if (device->logical) {
	    PyDict_SetItemString(dict,"logical",
				 o=PyString_FromString(device->logical));
	    Py_DECREF(o);
    } else
	    PyDict_SetItemString(dict,"logical", Py_None);
}

void addParallelInfo(PyObject *dict,struct parallelDevice * device)
{
    PyObject *o;
	
    if (device->pnpmodel) {
	    PyDict_SetItemString(dict,"pnpmodel",
				 o=PyString_FromString(device->pnpmodel));
	    Py_DECREF(o);
    } else
	    PyDict_SetItemString(dict,"pnpmodel",Py_None);
    if (device->pnpmfr) {
	    PyDict_SetItemString(dict,"pnpmfr",
				 o=PyString_FromString(device->pnpmfr));
	    Py_DECREF(o);
    } else
	    PyDict_SetItemString(dict,"pnpmfr",Py_None);
    if (device->pnpmodes) {
	    PyDict_SetItemString(dict,"pnpmodes",
				 o=PyString_FromString(device->pnpmodes));
	    Py_DECREF(o);
    } else
	    PyDict_SetItemString(dict,"pnpmodes",Py_None);
    if (device->pnpdesc) {
	    PyDict_SetItemString(dict,"pnpdesc",
				 o=PyString_FromString(device->pnpdesc));
	    Py_DECREF(o);
    } else
	    PyDict_SetItemString(dict,"pnpdesc",Py_None);
}

void addPCIInfo(PyObject *dict,struct pciDevice * device)
{
    PyObject *o;
    
    PyDict_SetItemString(dict,"vendorId", o=PyInt_FromLong(device->vendorId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"deviceId",o=PyInt_FromLong(device->deviceId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"subVendorId",
			 o=PyInt_FromLong(device->subVendorId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"subDeviceId",
			 o=PyInt_FromLong(device->subDeviceId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"pciType",
			 o=PyInt_FromLong(device->pciType));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"pcidom",
			 o=PyInt_FromLong(device->pcidom));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"pcibus",
			 o=PyInt_FromLong(device->pcibus));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"pcidev",
			 o=PyInt_FromLong(device->pcidev));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"pcifn",
			 o=PyInt_FromLong(device->pcifn));
    Py_DECREF(o);
}

void addPCMCIAInfo(PyObject *dict,struct pcmciaDevice * device)
{
    PyObject *o;
    
    PyDict_SetItemString(dict,"vendorId", o=PyInt_FromLong(device->vendorId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"deviceId",o=PyInt_FromLong(device->deviceId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"function",o=PyInt_FromLong(device->function));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"slot",o=PyInt_FromLong(device->slot));
    Py_DECREF(o);
}

void addSbusInfo(PyObject *dict,struct sbusDevice * device)
{
    PyObject *o;
    
    PyDict_SetItemString(dict,"width",o=PyInt_FromLong(device->width));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"height",o=PyInt_FromLong(device->height));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"freq",o=PyInt_FromLong(device->freq));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"monitor",o=PyInt_FromLong(device->monitor));
    Py_DECREF(o);
}

void addScsiInfo(PyObject *dict,struct scsiDevice * device)
{
    PyObject *o;
    
    PyDict_SetItemString(dict,"host",o=PyInt_FromLong(device->host));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"channel",o=PyInt_FromLong(device->channel));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"id",o=PyInt_FromLong(device->id));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"lun",o=PyInt_FromLong(device->lun));
    Py_DECREF(o);
}

void addSerialInfo(PyObject *dict,struct serialDevice * device)
{
    PyObject *o;
    
    if(device->pnpmfr) {
        PyDict_SetItemString(dict,"pnpmfr",
			     o=PyString_FromString(device->pnpmfr));
	Py_DECREF(o);
    } else {
        PyDict_SetItemString(dict,"pnpmfr",Py_None);
    }
    if(device->pnpmodel){
        PyDict_SetItemString(dict,"pnpmodel",o=PyString_FromString(device->pnpmodel));
	Py_DECREF(o);
    } else {
        PyDict_SetItemString(dict,"pnpmodel",Py_None);
    }
    if(device->pnpcompat) {
        PyDict_SetItemString(dict,"pnpcompat",o=PyString_FromString(device->pnpcompat));
	Py_DECREF(o);
    } else {
        PyDict_SetItemString(dict,"pnpcompat",Py_None);
    }
    if(device->pnpdesc) {
        PyDict_SetItemString(dict,"pnpdesc",o=PyString_FromString(device->pnpdesc));
	Py_DECREF(o);
    } else {
        PyDict_SetItemString(dict,"pnpdesc",Py_None);
    }
}

void addUsbInfo(PyObject *dict,struct usbDevice * device)
{
    PyObject *o;
    
    PyDict_SetItemString(dict,"usbclass",o=PyInt_FromLong(device->usbclass));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"usbsubclass",o=PyInt_FromLong(device->usbsubclass));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"usbprotocol",o=PyInt_FromLong(device->usbprotocol));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"usbbus",o=PyInt_FromLong(device->usbbus));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"usblevel",o=PyInt_FromLong(device->usblevel));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"usbport",o=PyInt_FromLong(device->usbport));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"vendorid",o=PyInt_FromLong(device->vendorId));
    Py_DECREF(o);
    PyDict_SetItemString(dict,"deviceid",o=PyInt_FromLong(device->deviceId));
    Py_DECREF(o);
    if (device->usbmfr) {
	  PyDict_SetItemString(dict,"usbmfr",o=PyString_FromString(device->usbmfr));
	  Py_DECREF(o);
    } else
	  PyDict_SetItemString(dict,"usbmfr",Py_None);
    if (device->usbprod) {
	  PyDict_SetItemString(dict,"usbprod",o=PyString_FromString(device->usbprod));
	  Py_DECREF(o);
    } else
	  PyDict_SetItemString(dict,"usbprod",Py_None);
}

/*
 * These are empty, and just used for consistency and possible
 * future expansion
 */

void addKbdInfo(PyObject *dict,struct keyboardDevice * device)
{
    return;
}

void addPsauxInfo(PyObject *dict,struct psauxDevice * device)
{
    return;
}

void addAdbInfo(PyObject *dict,struct adbDevice * device)
{
    return;
}

void addMacioInfo(PyObject *dict,struct macioDevice * device)
{
    return;
}

void addVioInfo(PyObject *dict,struct vioDevice * device)
{
    return;
}

void addS390Info(PyObject *dict,struct s390Device * device)
{
    return;
}

void addXenInfo(PyObject *dict,struct xenDevice * device)
{
    return;
}

PyObject * createDict(struct device * probedDevice)
{
    PyObject *dict;
    PyObject *o;

    dict = PyDict_New();

    if (probedDevice->desc) {
	o = PyString_FromString(probedDevice->desc);
	PyDict_SetItemString(dict,"desc",o);
	Py_DECREF(o);
    } else
	PyDict_SetItemString(dict,"desc",Py_None);
    if (probedDevice->driver) {
	o = PyString_FromString(probedDevice->driver);
	PyDict_SetItemString(dict,"driver",o);
	Py_DECREF(o);
    } else
	PyDict_SetItemString(dict,"driver",Py_None);
    if (probedDevice->device) {
	o = PyString_FromString(probedDevice->device);
	PyDict_SetItemString(dict,"device",o);
	Py_DECREF(o);
    } else
	PyDict_SetItemString(dict,"device",Py_None);
    PyDict_SetItemString(dict,"detached",o=PyInt_FromLong(probedDevice->detached)); Py_DECREF(o);
    PyDict_SetItemString(dict,"class",o=PyInt_FromLong(probedDevice->type)); Py_DECREF(o);
    PyDict_SetItemString(dict,"bus",o=PyInt_FromLong(probedDevice->bus)); Py_DECREF(o);
    PyDict_SetItemString(dict,"index",o=PyInt_FromLong(probedDevice->index)); Py_DECREF(o);
    if (probedDevice->classprivate && probedDevice->type == CLASS_NETWORK) {
	o = PyString_FromString((char *)probedDevice->classprivate);
	PyDict_SetItemString(dict,"hwaddr",o);
	Py_DECREF(o);
    }
    if (probedDevice->classprivate && probedDevice->type == CLASS_VIDEO) {
	o = PyString_FromString((char *)probedDevice->classprivate);
	PyDict_SetItemString(dict,"xdriver",o);
	Py_DECREF(o);
    }
    switch(probedDevice->bus){
    case BUS_ADB:
        addAdbInfo(dict,(struct adbDevice*)probedDevice);
        break;
    case BUS_DDC:
        addDDCInfo(dict,(struct ddcDevice*)probedDevice);
        break;
    case BUS_IDE:
        addIDEInfo(dict,(struct ideDevice*)probedDevice);
        break;
    case BUS_KEYBOARD:
        addKbdInfo(dict,(struct keyboardDevice*)probedDevice);
        break;
    case BUS_MACIO:
        addMacioInfo(dict,(struct macioDevice*)probedDevice);
        break;
    case BUS_PARALLEL:
        addParallelInfo(dict,(struct parallelDevice*)probedDevice);
        break;
    case BUS_PCI:
        addPCIInfo(dict,(struct pciDevice*)probedDevice);
        break;
    case BUS_PCMCIA:
	addPCMCIAInfo(dict,(struct pcmciaDevice*)probedDevice);
	break;
    case BUS_PSAUX:
        addPsauxInfo(dict,(struct psauxDevice*)probedDevice);
        break;
    case BUS_SBUS:
        addSbusInfo(dict,(struct sbusDevice*)probedDevice);
        break;
    case BUS_SCSI:
        addScsiInfo(dict,(struct scsiDevice*)probedDevice);
        break;
    case BUS_SERIAL:
        addSerialInfo(dict,(struct serialDevice*)probedDevice);
        break;
    case BUS_USB:
        addUsbInfo(dict,(struct usbDevice*)probedDevice);
        break;
    case BUS_VIO:
        addVioInfo(dict,(struct vioDevice*)probedDevice);
        break;
    case BUS_S390:
        addS390Info(dict,(struct s390Device*)probedDevice);
        break;
    case BUS_XEN:
        addXenInfo(dict,(struct xenDevice*)probedDevice);
        break;
    default:
        break;
    }
    return dict;
}

static PyObject * doProbe (PyObject * self, PyObject * args) {
    int class, bus, mode;
    struct device ** devices, ** tmp;
    PyObject * list;
    
    if (!PyArg_ParseTuple(args, "iii", &class, &bus, &mode))
	return NULL;

    devices = probeDevices(class, bus, mode);

    list = PyList_New(0);

    if (!devices)
	return list;
    
    tmp = devices;
    while (*tmp) {
	PyList_Append (list, createDict(*tmp));
	tmp++;
    }

    tmp = devices;
    while (*tmp) {
	(*tmp)->freeDevice (*tmp);
	tmp++;
    }

    free(devices);

    return list;
}


static PyMethodDef kudzuMethods[] = {
    { "probe", (PyCFunction) doProbe, METH_VARARGS, NULL },
    { NULL }
} ;

static void registerTable (PyObject * dict, TableEntry * table) {
    int i;
    PyObject *o;
    
    i = 0;
    while (table[i].name) {
	PyDict_SetItemString(dict, table[i].name,
			     o=PyInt_FromLong (table[i].value));
	Py_DECREF(o);
	i++;
    }
}

void init_kudzu (void) {
    PyObject * m, * dict;
    
    m = Py_InitModule("_kudzu", kudzuMethods);
    dict = PyModule_GetDict(m);

    registerTable (dict, classTable);
    registerTable (dict, busTable);
    registerTable (dict, modeTable);
	
    initializeDeviceList();
}
