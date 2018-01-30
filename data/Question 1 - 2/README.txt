/**************** Operational System ***************/
-> OS: Ubuntu 16.04.3 LTS
-> Kernel: Linux 4.4.86-rt99 (x86_64) / SMP PREEMPT RT 

/**************** OpenDaVINCI Execution ***************/
/***** ODSUPERCOMPONENT *****/
sudo odsupercomponent --cid=111 --freq=10 --realtime=20 --managed=simulation_rt --verbose=1

/***** BODYHUB *****/
./bodyhub --cid=111

/***** SENSORNODES *****/
./sensornode --cid=111 --id=X

/**************** Execution Scenario ***************/
 ___________________________________________________________
| Scenario | f(Hz) | Nb | Ns | Strategy | RTOS | Controlled |
|-----------------------------------------------------------|
|    01    |  10   | 01 | 01 |  Replc.  | true |    yes     |
|    02    |  10   | 01 | 03 |  Replc.  | true |    yes     |
|    03    |  10   | 01 | 05 |  Replc.  | true |    yes     |
|    04    |  10   | 01 | 10 |  Replc.  | true |    yes     |
|    05    |  10   | 01 | 20 |  Replc.  | true |    yes     |
|    06    |  10   | 01 | 01 |  3 of 5  | true |    yes     |
|    07    |  10   | 01 | 03 |  3 of 5  | true |    yes     |
|    08    |  10   | 01 | 05 |  3 of 5  | true |    yes     |
|    09    |  10   | 01 | 10 |  3 of 5  | true |    yes     |
|    10    |  10   | 01 | 20 |  3 of 5  | true |    yes     |
------------------------------------------------------------- 

/**************** Output Data Format ***************/

--BodyHub
 _____
|Tref |
 -----
 ______________________________________________________________________________
| Sender | Actual         | Emergency? | Tbodyhub.  | Tsensornode. | delta (s) |
| Sensor | Patient Status |            |  processed |  collected   |           |
 ------------------------------------------------------------------------------

*Tref = Reference timestamp collected on bodyhub instantiation, related to 01/01/1970 00:00:00
*Sender Sensor = Sender ID sensor
*Actual Patient Status = "good"/"medium"/"bad"
*Emergency = Has the system detected critical status ? "true" or "false"
*Tbodyhub.processed = Instant of data processing, related to Tref (Tproc-Tref)
*Tsensornode.collected = Instant of data collected in sensornode, related to Tref (Tcol-Tref)
*delta = Tbodyhub.processed - Tsensornode.collected 

--SensorNodes
 _____
|Tref |
 -----
 _________________________________________________________________________________
| ID | Actual Sensor  | Collected     | Data   | Tsensornode. | Tinst sensornode. |
|    | Risk Status    | Sensor Status | Sent?  |  collected   | collected         |
 ---------------------------------------------------------------------------------

*Tref = Reference timestamp collected on sensornode instantiation, related to 01/01/1970 00:00:00
*ID = Sensornode instance ID
*Actual Sensor Risk Status = "high"/"moderate"/"low"
*Collected Sensor Status = "high"/"moderate"/"low" - Collected in the cycle
*Data Sent = Has the sensornode sent data? "true" or "false"
*Tsensornode.collected = Instant of data collected, related to Tref (Tcol-Tref)
*Tinst sensornode.collected = Instant of data collected in sensornode, related to 01/01/1970 00:00:00 

