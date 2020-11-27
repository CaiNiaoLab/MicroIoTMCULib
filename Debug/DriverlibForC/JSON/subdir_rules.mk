################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DriverlibForC/JSON/%.obj: ../DriverlibForC/JSON/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_20.2.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="E:/CaiNiaoLabforgit/MicorIoT" --include_path="C:/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_20.2.3.LTS/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/netapps" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/netapps/mqtt/client" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/netapps/mqtt/common" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/netapps/mqtt/platform" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/netapps/mqtt/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/third_party/FreeRTOS/source/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/third_party/FreeRTOS/source" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/third_party/FreeRTOS/source/portable/CCS/ARM_CM3" --include_path="E:/CaiNiaoLabforgit/MicorIoT/Board/CC3200" --include_path="E:/CaiNiaoLabforgit/MicorIoT/Application/GateWayApp" --include_path="E:/CaiNiaoLabforgit/MicorIoT/DriverlibForC/Devices" --include_path="E:/CaiNiaoLabforgit/MicorIoT/DriverlibForC/JSON" --include_path="E:/CaiNiaoLabforgit/MicorIoT/DriverlibForC/RingBuffer" --include_path="E:/CaiNiaoLabforgit/MicorIoT/DriverlibForHal" --include_path="E:/CaiNiaoLabforgit/MicorIoT/DriverlibForSensor" --include_path="E:/CaiNiaoLabforgit/MicorIoT/NetWork/MQTT" --define=ccs --define=cc3200 --define=USE_FREERTOS --define=SL_PLATFORM_MULTI_THREADED -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="DriverlibForC/JSON/$(basename $(<F)).d_raw" --obj_directory="DriverlibForC/JSON" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


