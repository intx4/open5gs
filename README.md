
## Documentation

[https://open5gs.org/open5gs/docs/](https://open5gs.org/open5gs/docs/)

## This Fork
This fork simply enables logging in the ```AMF``` component for IEF Associations and Deassociations events (ETSI TS 133127 and 133128)

## Lawful Interception
This repo is a submodule of the [P3LI5 project](https://github.com/intx4/P3LI5) and is meant for deployment with Docker

## Build

### open5gs
You can skip the section about "Getting MongoDB" [guide](https://open5gs.org/open5gs/docs/guide/02-building-open5gs-from-sources/)

### UERANSIM
- Clone [UERANSIM](https://github.com/intx4/ueransimLI)
- Just run ```make``` in ```/ueransimLI```. If you have an older version of make, you can install a [new one](https://cmake.org/download/) by downloading the ```.sh``` script for your Linux arch.

## Run

### UERANSIM - Populating the DB
After finishing the guide, you can populate the DB with subscribers using ```populate_db.py```. Use ```-h``` for understanding how it works.

### open5gs - Run Core
-   run ```run_core.sh```
-   if restart is needeed, run ```cleanup.sh```

### UERANSIM - Run RAN
Follow UERANSIM README

## License

- Open5GS Open Source files are made available under the terms of the GNU Affero General Public License ([GNU AGPL v3.0](https://www.gnu.org/licenses/agpl-3.0.html)).
- [Commercial licenses](https://open5gs.org/open5gs/support/) are also available from [NextEPC, Inc.](https://nextepc.com)
