
һ��ģ��

Track��Ŀ¼�·�����ģ��

1��os
    Ӳ��ϵͳ���������ı���ӿ� (task+timer+log)

2��drv 
    Ӳ��������

/***** ��os+drv����֤�ɲ���PCBA   *****/
3��at
    ��ص������ ��at+sms+call+nvram+factory��
    ��װMTK�ײ���ؽӿڣ������ϲ���չ����

4��cmd


5��soc 
    ���̨�������ɿ�ͨ��

6��fun
    ��ƽ̨�޹ػ����ϵ�ͨ�ù��ܽӿ�

7��cust
    ����Ŀ��ͬ�Ķ��Ʋ��֣���ҵ���߼���

8��res
    �ַ�����������Դ�ļ�


��������

1��ÿ��ģ���ļ������ٷ֣�ģ��_inc���ͣ�ģ��_src���������ļ���

2��ģ����⺯��ȫ�����У��ļ�����Ϊ ��track_ģ�飩������������track_ģ��_XX����ʾȫ�ֺ��������õ����⺯���綨ʱ���ȿ�ʡȥ��_ģ�飩

3��ģ���ڲ��������ļ��������ܻ���Ϊ (track_ģ��_����_XX)������������Ϊ��track_XX_XX����ǰ׺track_����ģ��������ȫ�ֺ������֣���static����

4��ȫ�ֺ�Ϊ __GPS_TRACK__�����Ǳ�����RF��������Ӳ���ӿ���صģ��԰���Ŀ���֣�����һ���Թ��ܺ껮��


�������ܻ���˵��

1��os

track_main.c 		//�Զ��庯�����
track_timer.c		//��ʱ����������ȷ��ʱ��

2��drv
track_drv.c		//��������ӿڣ�����track_drv.h�ɶ������ã���������ʹ��
track_drv_pmu.c	//�����ع����Լ��������
track_drv_eint_gpio.c	//���ж����ã�GOIO�������
track_drv_gps_decode.c	//������GPS����
track_drv_gps_drive.c	//GPS��������Ҫ�ǿ��ƿ����Լ�������task�Ľ���
track_drv_gps_epo.c	//EPO���ܣ��Զ�������ע��EPO������
track_drv_key.c		//�Զ�����̴���
track_drv_lbs_data.c	//LBS���վ��Ϣ
track_drv_led.c		//LED����
track_drv_sensor.c		//sensor�����Ĺ�������
track_drv_system_param.c	//ϵͳӲ��������ȡ
track_drv_test.c		//��������ָ�����PCBA��֤
track_drv_watch_dog.c	//152ι���ź�
track_drv_sensor.c		//sensor����

3��at


4��cmd


5��soc 


6��fun


7��cust


8��res


--------------------------------
�̿�

19:32 2013/4/12


