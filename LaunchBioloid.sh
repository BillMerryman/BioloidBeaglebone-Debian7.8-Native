#!/bin/sh

echo "Running overlayPinConfig.sh to set pinmux and start pwm"; \
./SupportingMaterial/OverlayScripts/overlayPinConfig.sh; \
echo "Running GRB422-QVGA.sh to configure camera"; \
./SupportingMaterial/CameraManipulationScripts/GRB422-QVGA.sh; \
./SupportingMaterial/CameraManipulationScripts/SetViewport.sh; \
echo "Launching BioloidBeaglebone"; \
./BioloidBeaglebone PRU_0/gen/text_0.bin PRU_0/gen/data_0.bin PRU_1/gen/text_1.bin PRU_1/gen/data_1.bin ./SupportingMaterial/MotionFiles/FlexTest.mtn ./SupportingMaterial/MLFiles/MobileNetSSD_deploy.prototxt.txt ./SupportingMaterial/MLFiles/MobileNetSSD_deploy.caffemodel;
