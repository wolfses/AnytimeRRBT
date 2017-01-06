# Anytime RRBT

##Requirement
QT4

V-REP

##Quick Start(MacOSX)
```bash
git clone https://github.com/wolfses/AnytimeRRBT.git
cd AnytimeRRBT/pathPlanningPlugin
qmake
make
cp libv_repExtPathPlanning.1.0.0.dylib /Applications/V-REP_PRO_EDU_V3_2_0_Mac/vrep.app/Contents/MacOS
```

!Be sure of the version of V-REP

##Quick Start(Ubuntu)
```bash
git clone https://github.com/wolfses/AnytimeRRBT.git
cd AnytimeRRBT/pathPlanningPlugin
qmake
make
```

Then, you can get lots of object files and four libv_repExtPathPlanning.so files.

Among them, move libv_repExtPathPlanning.so.1.0.0 to V-REP directory.

!After that, you have to rename libv_repExtPathPlanning.so.1.0.0 to libv_repExtPathPlanning.so
!qmake can be ignored on linux system but recommended




Related paper :
http://sglab.kaist.ac.kr/anytimeRRBT/
