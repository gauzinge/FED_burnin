Code project for CMS Pixel FED burn-in testing

contains some classes borrowed from [[Ph2_ACF][www.gitlab.cern.ch/cmstkph2/Ph2_ACF.git]], an adapted uHAL address table for Ph1 Pix FED and a small application to run readout & data frame checking in an infinite loop.



#### Setup on SLC5/6

1. The software uses c++11 features, therefore you need a gcc version that supports this. gcc4.4 should work, gcc 4.8 can be obtained following these insructions:

          $> sudo yum install devtoolset-2
          $> sudo ln -s /opt/rh/devtoolset-2/root/usr/bin/* /usr/local/bin/
          $> hash -r

This should give you gcc 4.8.1:

          $> gcc --version

2. Install uHAL  version 2.4 with AMC13 support (optional if you want to use the AMC13 features):

for SLC6:

          $> wget https://svnweb.cern.ch/trac/cactus/export/30508/trunk/scripts/release/cactus-amc13.slc6.x86_64.repo
          sudo cp cactus-amc13.slc6.x86_64.repo /etc/yum.repos.d/cactus-amc13.repo

(You may need the --no-check-certificate)

          $> sudo yum clean all
          $> sudo yum groupinstall uhal
          $> sudo yum groupinstall amc13

Note: You may also need to set the environment variables (or source setup.sh provided with the repo):

          $> export LD_LIBRARY_PATH=/opt/cactus/lib:$LD_LIBRARY_PATH
          $> export PATH=/opt/cactus/bin:$PATH

#### Compilation

1. clone the github repo:    

          $> git clone https://github.com/gauzinge/FED_burnin.git

2. Source the setup.sh script

          $> source setup.sh

3. run make

#### Running the Software

1. Launch 

          $> run settings/HWDescription.xml

to read data from the readout FIFOs of the FED, this was mainly developed for the long term soak test. Details about configuration in the HWDescription.xml file below.

2. Launch

          $> amc13config settings/HWDescription.xml

to apply a configuration defined in the xml file to the amc13 - this requires the AMC13 software installed.

3. Launch

          $> fedconfig settings/HWDescription.xml

to run basic tests with the FED and apply an amc13 configuration. This binary allows to configure the FED & FITEL FMCs, read the ADC of the FMC, run the phase finding and dump spy / readout FIFO contents.

4. Launch

          $> fpgaconfig --help

To manage FW on the SD card of the CTA. Note that this code can convert .bit files to .bin files. Also note that the CTA will always boot from an image called "GoldenImage.bin" which has to be present on the SD card.

#### HWDescription files

The complete configuration of the HW setup is defined in an xml file. A default file can be found in settins. This files is used to define the configuration of the AMC13 (if installed with AMC13 software) & it's IP addresses & address tables. The FED config is also specified (IP address, address table, register settings, FITEL configuration). This is an exampel configuration for such a file:

````xml

<HwDescription>
    <AMC13>
        <connection id="T1" uri="chtcp-2.0://cmsuppixch:10203?target=amc13T101:50001" address_table="file://settings/AMC13XG_T1.xml" />
        <connection id="T2" uri="chtcp-2.0://cmsuppixch:10203?target=amc13T201:50001" address_table="file://settings/AMC13XG_T2.xml" />
        <AMCmask enable="9" />
        <Register tounge="T1" name="test.test">0</Register>
        <BGO command="15" repeat="1" prescale="0" bx="100" />
        <Trigger local="1" mode="1" rate="100" burst="100" rules="0" />
        <TTCSimulator>0</TTCSimulator>
    </AMC13>

```

The connection nodes specify the required values for uHAL and the AMC13. The AMCmask node allows to selectively enable TTC links from the AMC13 to specific AMCs. Register nodes can be used to write the value to any given register of the AMC13 for non-standard configuration. BGO and Trigger nodes allow to configure up to 4 BGOs and one Trigger config. Finally, the TTCSimulator value has to be set to 0 to disable and 1 to enable.

````xml
<PixFED Id="0" boardType="CTA">
    <connection id="board0" uri="chtcp-2.0://cmsuppixch:10203?target=devfed:50001" address_table="file://settings/address_table.xml" />
    <!-- Trigger -->
    <!--Used to set the CLK input to the TTC clock from the BP - 3 is XTAL, 0 is BP-->
    <Register name="ctrl.ttc_xpoint_A_out3">0</Register>
    <Register name="pixfed_ctrl_regs.TRIGGER_SEL">0</Register>
    <Register name="pixfed_ctrl_regs.INTERNAL_TRIG_EN">0</Register>
    <Register name="pixfed_ctrl_regs.data_type">0</Register>
    <!-- TBM MASK -->
    <Register name="pixfed_ctrl_regs.TBM_MASK_1">0xFFFFFFFC</Register>
    <Register name="pixfed_ctrl_regs.TBM_MASK_2">0xFFFFFFFF</Register>
    <Register name="pixfed_ctrl_regs.TBM_MASK_3">0xFFFFFFFF</Register>
    <!-- FITELS -->
    <Fitel_Files path="./settings/" />
    <!--<Fitel FMC="0" Id="0" file="FMC0_Fitel0.txt" />-->
    <!--<Fitel FMC="0" Id="1" file="FMC0_Fitel1.txt" />-->
    <Fitel FMC="1" Id="0" file="FMC1_Fitel0.txt" />
    <Fitel FMC="1" Id="1" file="FMC1_Fitel1.txt" />
</PixFED>
```
The connections node is self-explanatory, adapt according to the uHAL twiki. Register nodes allow to control the configuration of the FED, more specifically:

"ctrl.ttc.xpoint_A_out3" set to 3 uses the ttc clock via the uTCA backplane whereas 1 sets it to the CTA's internal oscillator. The "TRIGGER_SEL" register is set to 0 for TTC triggers and to 1 for internal triggers. "data_type" is 0 for real data from modules, 1 for internally generated patterns after the TBM FIFOs and 2 for fake data generated at the input of the TBM FIFOs. The TBM masks (1 bit per TBM core) allows to mask and control which TBM FIFOs are drained in the global readout FIFO. The Fitel nodes allow to point to the config files for the FITEL receivers and the path prefix can be used to specify the directory. 


````xml
<Settings>
    <Setting name="NTBM">2</Setting>
    <!--<Setting name="ChannelOfInterest">0</Setting>-->
    <Setting name="ChannelOfInterest">0</Setting>
    <Setting name="BlockSize">256</Setting>
    <!--if set to 0, loop will run forever-->
    <!--<Setting name="NAcq">100</Setting>-->
</Settings>

```

Finally the settins node allows to specify some parameters. NTBM is not strictly needed for operation with real module data, the channel of interest determines which spy FIFOs are read out and the Block size defines the size of the readout memory blocks used (in 32 bit words).

