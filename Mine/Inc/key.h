/****************************************************************
 * ����ɨ����
 * File  : key.c
 * Author: chenling
 * Date  : 2021.7
 * Func  : ����ɨ���ܣ�֧���¼�������/˫��/����/�ſ�/����/����
 ****************************************************************
 *  ʹ����, ��������, �ֱ�Ϊ�������������¼���Ӧ����btn1Func��btn2Func
 *  Ȼ����Ϊ��������Key_init���������ѭ���е���KEY_keyscan���ɡ�
 *  ʾ������:
 *    void btn1Func(u8 event_id){ 
 *       switch(event_id){
 *       case KEY_EVENT_CLICK:
 *           // ����1��������
 *           // ...
 *           break;
 *       case KEY_EVENT_LONG_PRESS:
 *           // ����1��������
 *           // ...
 *           break;
 *       } 
 *    }
 * 
 *    void btn2Func(u8 event_id){ 
 *       switch(event_id){
 *       case KEY_EVENT_CLICK:
 *           // ����2��������
 *           // ...
 *           break;
 *       } 
 *    }
 * 
 *    void main(){
 *      // �����������¼�����������KEY_init
 *      KEY_init(btn1Func, btn2Func);
 *      while(1){
 *          // ����ѭ����ɨ�谴����һ����⵽�¼���ص������¼�����
 *          KEY_keyscan();
 *      }
 *    }
 *****************************************************************/
#ifndef __KEY_H_
#define __KEY_H_

// for G&W
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_A 5
#define KEY_B 6
#define KEY_GAME 7
#define KEY_TIME 8
#define KEY_PAUSE 9
#define KEY_POWER 10



// �����ص��¼�����ָ�����Ͷ��� ����Ҫ�޸ģ�
// ������һ����Ϊ��pBtnEventFunc���ĺ���ָ�����ͣ�����ָ��ĺ���û�з���ֵ����һ���������ڽ��ܲ�ͬ�¼�(��������¼��궨��)
typedef void (*pBtnEventFunc)(u8);

// �¼�ID����
#define KEY_EVENT_CLICK         0 //���������²���ָ��ʱ���ڷſ��󴥷�һ�Σ����¼��밴��/̧���¼�ֻ��ѡ����һ
#define KEY_EVENT_DOWN          1 //����
#define KEY_EVENT_UP            2 //̧��
#define KEY_EVENT_DBCLICK       3 //˫��������������Ч�ĵ����󴥷�һ�Σ����¼�����ʱ�����˫���¼�֮ǰ���Ѿ�ȷ����Ч�ĵ����¼����ᱻ����
#define KEY_EVENT_LONG_PRESS    4 //��������ס����Ҳֻ�ᴥ��һ�Σ�
#define KEY_EVENT_KEEPING_PRESS 5 //��ס�����������ض�ʱ��������������

void KEY_init(
    pBtnEventFunc btn1Func,
    pBtnEventFunc btn2Func,
    pBtnEventFunc btn3Func,
    pBtnEventFunc btn4Func,
    pBtnEventFunc btn5Func,
    pBtnEventFunc btn6Func,
    pBtnEventFunc btn7Func,
    pBtnEventFunc btn8Func,
    pBtnEventFunc btn9Func,
    pBtnEventFunc btn10Func
    );

// ��ѭ������øú���
void KEY_keyscan();

#endif