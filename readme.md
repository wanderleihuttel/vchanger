vchanger is designed to implement a disk-based virtual autochanger device for
use with Bacula(TM), an open-source network backup solution.

Please see the vchangerHowto.html in the doc directory of the vchanger source
for further documentation and installation instructions.

I've made a bit change in function CreateVolumes to modify the way to generate volume names,
because by default when vchanger creates volumes doesn't keep the correct order in "list media"
```
Before changes:

vchanger /etc/vchanger/vchanger.conf createvols 3 15 1

Pool: Scratch
+---------+----------------------------+-----------+
| MediaId | VolumeName                 | VolStatus |
+---------+----------------------------+-----------+
|     111 | StorageVirtualChanger_3_1  | Append    |
|     112 | StorageVirtualChanger_3_10 | Append    |
|     113 | StorageVirtualChanger_3_11 | Append    |
|     114 | StorageVirtualChanger_3_12 | Append    |
|     115 | StorageVirtualChanger_3_13 | Append    |
|     116 | StorageVirtualChanger_3_14 | Append    |
|     117 | StorageVirtualChanger_3_15 | Append    |
|     118 | StorageVirtualChanger_3_2  | Append    |
|     119 | StorageVirtualChanger_3_3  | Append    |
|     120 | StorageVirtualChanger_3_4  | Append    |
|     121 | StorageVirtualChanger_3_5  | Append    |
|     122 | StorageVirtualChanger_3_6  | Append    |
|     123 | StorageVirtualChanger_3_7  | Append    |
|     124 | StorageVirtualChanger_3_8  | Append    |
|     125 | StorageVirtualChanger_3_9  | Append    |
+---------+----------------------------+-----------+

vchanger /etc/vchanger/vchanger.conf createvols 3 15 1 --label=Volume-Daily

Pool: Scratch
+---------+-----------------+-----------+
| MediaId | VolumeName      | VolStatus |
+---------+-----------------+-----------+
|     126 | Volume-Daily_1  | Append    |
|     127 | Volume-Daily_10 | Append    |
|     128 | Volume-Daily_11 | Append    |
|     129 | Volume-Daily_12 | Append    |
|     130 | Volume-Daily_13 | Append    |
|     131 | Volume-Daily_14 | Append    |
|     132 | Volume-Daily_15 | Append    |
|     133 | Volume-Daily_2  | Append    |
|     134 | Volume-Daily_3  | Append    |
|     135 | Volume-Daily_4  | Append    |
|     136 | Volume-Daily_5  | Append    |
|     137 | Volume-Daily_6  | Append    |
|     138 | Volume-Daily_7  | Append    |
|     139 | Volume-Daily_8  | Append    |
|     140 | Volume-Daily_9  | Append    |
+---------+-----------------+-----------+


After changes:

vchanger /etc/vchanger/vchanger.conf createvols 3 15 1

Pool: Scratch
+---------+------------------------------+-----------+
| MediaId | VolumeName                   | VolStatus |
+---------+------------------------------+-----------+
|     156 | StorageVirtualChanger_3_0001 | Append    |
|     157 | StorageVirtualChanger_3_0002 | Append    |
|     158 | StorageVirtualChanger_3_0003 | Append    |
|     159 | StorageVirtualChanger_3_0004 | Append    |
|     160 | StorageVirtualChanger_3_0005 | Append    |
|     161 | StorageVirtualChanger_3_0006 | Append    |
|     162 | StorageVirtualChanger_3_0007 | Append    |
|     163 | StorageVirtualChanger_3_0008 | Append    |
|     164 | StorageVirtualChanger_3_0009 | Append    |
|     165 | StorageVirtualChanger_3_0010 | Append    |
|     166 | StorageVirtualChanger_3_0011 | Append    |
|     167 | StorageVirtualChanger_3_0012 | Append    |
|     168 | StorageVirtualChanger_3_0013 | Append    |
|     169 | StorageVirtualChanger_3_0014 | Append    |
|     170 | StorageVirtualChanger_3_0015 | Append    |
+---------+------------------------------+-----------+
*

This way I can define if I use "-" (hyphen) or "_" (underscore) to label volumes and the volume numbers stay in correct order.

vchanger /etc/vchanger/vchanger.conf createvols 3 15 1 --label=Volume-Daily-

Pool: Scratch
+---------+-------------------+-----------+
| MediaId | VolumeName        | VolStatus |
+---------+-------------------+-----------+
|     171 | Volume-Daily-0004 | Append    |
|     172 | Volume-Daily-0005 | Append    |
|     173 | Volume-Daily-0006 | Append    |
|     174 | Volume-Daily-0007 | Append    |
|     175 | Volume-Daily-0008 | Append    |
|     176 | Volume-Daily-0009 | Append    |
|     177 | Volume-Daily-0010 | Append    |
|     178 | Volume-Daily-0011 | Append    |
|     179 | Volume-Daily-0012 | Append    |
|     180 | Volume-Daily-0013 | Append    |
|     181 | Volume-Daily-0014 | Append    |
|     182 | Volume-Daily-0015 | Append    |
+---------+-------------------+-----------+
*

This way I can define if I use "-" (hyphen) or "_" (underscore) to label volumes and the volume numbers are in correct order.

vchanger /etc/vchanger/vchanger.conf createvols 3 15 1 --label=Volume_Daily_

Pool: Scratch
+---------+-------------------+-----------+
| MediaId | VolumeName        | VolStatus |
+---------+-------------------+-----------+
|     183 | Volume_Daily_0001 | Append    |
|     184 | Volume_Daily_0002 | Append    |
|     185 | Volume_Daily_0003 | Append    |
|     186 | Volume_Daily_0004 | Append    |
|     187 | Volume_Daily_0005 | Append    |
|     188 | Volume_Daily_0006 | Append    |
|     189 | Volume_Daily_0007 | Append    |
|     190 | Volume_Daily_0008 | Append    |
|     191 | Volume_Daily_0009 | Append    |
|     192 | Volume_Daily_0010 | Append    |
|     193 | Volume_Daily_0011 | Append    |
|     194 | Volume_Daily_0012 | Append    |
|     195 | Volume_Daily_0013 | Append    |
|     196 | Volume_Daily_0014 | Append    |
|     197 | Volume_Daily_0015 | Append    |
+---------+-------------------+-----------+
```
