#This file for track

# Define source file lists to SRC_LIST

# os
SRC_LIST = track\os\os_src\track_os_main.c\
           track\os\os_src\track_os_timer.c\
           track\os\os_src\track_os_log.c\
           track\os\os_src\track_os_ell.c\
           track\os\os_src\track_os_paramete.c\
           track\os\os_src\track_os_file.c\
           track\os\os_src\track_os_parameter_handle.c


# SRC_LIST += track\mtk_lib\mtk_lib.c 

# drv
SRC_LIST +=track\drv\drv_src\track_drv.c\
           track\drv\drv_src\track_drv_watch_dog.c\
           track\drv\drv_src\track_drv_pmu.c\
           track\drv\drv_src\track_drv_led.c\
           track\drv\drv_src\track_drv_gps_drive.c\
           track\drv\drv_src\track_drv_gps_decode.c\
           track\drv\drv_src\track_drv_gps_epo.c\
           track\drv\drv_src\track_drv_eint_gpio.c\
           track\drv\drv_src\track_drv_lbs_data.c\
           track\drv\drv_src\track_drv_test_pcba.c\
           track\drv\drv_src\track_drv_system_param.c\
           track\drv\drv_src\track_drv_sensor.c\
           track\drv\drv_src\track_drv_key.c\
           track\drv\drv_src\track_drv_lcd.c\
           track\drv\drv_src\track_drv_uart.c\
           track\drv\drv_src\track_drv_wifi.c\
           track\drv\drv_src\track_drv_bt.c\
           track\drv\drv_src\track_drv_rs485.c\
           track\drv\drv_src\track_drv_uart1_mux.c\
           track\drv\drv_src\track_drv_system_interface.c\
           track\drv\drv_src\Track_drv_network_license.c\
           track\drv\drv_src\track_drv_mcu_sw_update.c\
           


# cmd
SRC_LIST +=track\at\at_src\track_at.c\
           track\at\at_src\track_at_context.c\
           track\at\at_src\track_at_call.c\
           track\at\at_src\track_at_sms.c\
           track\at\at_src\track_at_sim.c

# soc
SRC_LIST +=track\soc\soc_src\track_soc.c\
           track\soc\soc_src\track_soc_apn.c\
           track\soc\soc_src\track_soc_Lcon.c

# fun
SRC_LIST +=track\fun\fun_src\track_fun_common.c\
           track\fun\fun_src\track_fun_crc.c\
           track\fun\fun_src\track_fun_SHA.c\
           track\fun\fun_src\track_fun_cJSON.c\
           track\fun\fun_src\track_fun_md5.c\
           track\fun\fun_src\track_fun_hmac.c\
           track\fun\fun_src\track_fun_sha1.c

# cmd
SRC_LIST +=track\cmd\cmd_src\track_cmd.c\
           track\cmd\cmd_src\track_cmd_action.c\
           track\cmd\cmd_src\track_cmd_factory_test.c\
           track\cmd\cmd_src\track_cmd_auto_test.c

# cust
SRC_LIST +=track\cust\cust_src\track_cust_main.c\
           track\cust\cust_src\track_cust_led.c\
           track\cust\cust_src\track_cust_sensor.c\
           track\cust\cust_src\track_cust_other.c\
           track\cust\cust_src\track_cust_nvram.c\
           track\cust\cust_src\track_cust_status.c\
           track\cust\cust_src\track_cust_audio.c\
           track\cust\cust_src\track_cust_key.c\
           track\cust\cust_src\track_cust_lcd.c\
           track\cust\cust_src\track_cust_gps.c\
           track\cust\cust_src\track_cust_agps.c\
           track\cust\cust_src\track_cust_paket.c\
           track\cust\cust_src\track_cust_backup.c\
           track\cust\cust_src\track_cust_net.c\
           track\cust\cust_src\track_cust_net_server.c\
           track\cust\cust_src\track_cust_cmd.c\
           track\cust\cust_src\track_cust_filter.c\
           track\cust\cust_src\track_cust_gpsmode.c\
           track\cust\cust_src\track_cust_sim_backup.c\
           track\cust\cust_src\track_cust_timezone_auto.c\
           track\cust\cust_src\track_cust_module_fence.c\
           track\cust\cust_src\track_cust_module_get_addr.c\
           track\cust\cust_src\track_cust_module_GPRS_block.c\
           track\cust\cust_src\track_cust_module_low_power.c\
           track\cust\cust_src\track_cust_module_oil_cut.c\
           track\cust\cust_src\track_cust_module_oil_detection.c\
           track\cust\cust_src\track_cust_module_obd.c\
           track\cust\cust_src\track_cust_module_obd2.c\
           track\cust\cust_src\track_cust_module_extpower.c\
           track\cust\cust_src\track_cust_module_power_fails.c\
           track\cust\cust_src\track_cust_module_shocks.c\
           track\cust\cust_src\track_cust_module_SOS.c\
           track\cust\cust_src\track_cust_module_spatial_displacement.c\
           track\cust\cust_src\track_cust_module_speed_limit.c\
           track\cust\cust_src\track_cust_module_alarm.c\
           track\cust\cust_src\track_cust_module_sms_relay.c\
           track\cust\cust_src\track_cust_module_blind.c\
           track\cust\cust_src\track_cust_module_motor_lock.c\
           track\cust\cust_src\track_cust_module_fakecell.c\
           track\cust\cust_src\track_cust_gps_filter.c\
           track\cust\cust_src\Track_cust_cmd_online.c\
           track\cust\cust_src\track_cust_paket_cq.c\
           track\cust\cust_src\track_cust_mode.c\
           track\cust\cust_src\track_cust_ussd.c\
           track\cust\cust_src\track_cust_net_server_file.c\
           track\cust\cust_src\track_cust_wifi.c\
           track\cust\cust_src\track_cust_bt.c\
           track\cust\cust_src\track_cust_netlog.c\
           track\cust\cust_src\track_cust_SIM_ChinaUnicom.c\
           track\cust\cust_src\track_cust_work_mode.c\
           track\cust\cust_src\Track_cust_fall_down.c\
           track\cust\cust_src\track_cust_paket_bb.c\
           track\cust\cust_src\track_cust_atgm_update.c\
           track\cust\cust_src\track_cust_dasouche.c
#  stl
SRC_LIST += track\fun\fun_src\c_RamBuffer.c\
            track\fun\fun_src\c_vector.c\
            track\fun\fun_src\c_string.c

#  mqtt
SRC_LIST += track\mqtt\MQTT_main.c\
            track\mqtt\MQTTConnectClient.c\
            track\mqtt\MQTTConnectServer.c\
            track\mqtt\MQTTDeserializePublish.c\
            track\mqtt\MQTTFormat.c\
            track\mqtt\MQTTPacket.c\
            track\mqtt\MQTTSerializePublish.c\
            track\mqtt\MQTTSubscribeClient.c\
            track\mqtt\MQTTSubscribeServer.c\
            track\mqtt\MQTTUnsubscribeClient.c\
            track\mqtt\MQTTUnsubscribeServer.c\
            track\mqtt\MQTT_transport.c

# Only need to be increased .c file in the above

#  Define include path lists to INC_DIR
INC_DIR =  track\os\os_inc\
           track\drv\drv_inc\
           track\at\at_inc\
           track\soc\soc_inc\
           track\cmd\cmd_inc\
           track\cust\cust_inc\
           track\fun\fun_inc\
           track\nvram\
           track_agps\inc\
           gps\mnl\include\
           hal\peripheral\inc\
           C_STL\C_STLInc\
           kal\include\
           interface\fs\
           interface\hal\peripheral \
           sss\interface\inc \
           hal\system\GFH\public  \
           hal\system\GFH\public \
           track\mqtt


# Define the specified compile options to COMP_DEFS
COMP_DEFS = __GPS_TRACK__

# Define the source file search paths to SRC_PATH
SRC_PATH = track\os\os_src\
           track\drv\drv_src\
           track\at\at_src\
           track\soc\soc_src\
           track\cmd\cmd_src\
           track\cust\cust_src\
           track\fun\fun_src\
           track\mqtt\common\
           track\mqtt\MQTT\
           track\mqtt\MQTTPacket\
           track\mqtt\platform

#WATDOG
ifdef WATDOG
    ifneq ($(strip $(WATDOG)),NONE)
      COMP_DEFS += __WATCH_DOG__
      ifeq ($(strip $(WATDOG)),PULSE)
        COMP_DEFS += __WDT_PLUSE__
      endif
      ifeq ($(strip $(WATDOG)),IIC)
        COMP_DEFS += __WDT_USE_I2C__
      endif
  endif
endif
