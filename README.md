
## Documentation

[https://open5gs.org/open5gs/docs/](https://open5gs.org/open5gs/docs/)

## Build

### open5gs
You can skip the section about "Getting MongoDB" [guide](https://open5gs.org/open5gs/docs/guide/02-building-open5gs-from-sources/)

### UERANSIM
Just run ```make``` in ```/UERANSIM```. If you have an older version of make, you can install a [new one](https://cmake.org/download/) by downloading the ```.sh``` script for your Linux arch.

## Run

### UERANSIM - Populating the DB
After finishing the guide, you can populate the DB with subscribers using ```populate_db.py```. Use ```-h``` for understanding how it works.

### open5gs - Run Core and userplane
-   first run ```run_core.sh```
-   then run ```run_uplane.sh```
-   if restart is needeed, run ```cleanup.sh```

### UERANSIM - RAN
-   cd ```UERANSIM/build```
-   run ```./nr-gnb -c ../config/open5gs-gnb-demo.yalm```
-   run ```sudo python3 animate.py``` for starting the UEs and triggering "realistic" associations and deassociations
## License

- Open5GS Open Source files are made available under the terms of the GNU Affero General Public License ([GNU AGPL v3.0](https://www.gnu.org/licenses/agpl-3.0.html)).
- [Commercial licenses](https://open5gs.org/open5gs/support/) are also available from [NextEPC, Inc.](https://nextepc.com)
