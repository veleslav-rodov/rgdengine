//TestTrack.h
#pragma once
#include "input/input.h"
#include "input/helper.h"
#include "math/mathTargetCamera.h"
#include "math/mathSplines.h"
#include "math/mathTrack.h"

class TestTrack: public game::IDynamicObject
{
public:
    TestTrack();
    ~TestTrack();

protected:
    void initInput();
    void initCamera();
    void initLights();

    void update (float dt);

    void onEsc   ();
    void onReset ();
    void onXAxis (int dx);
    void onYAxis (int dy);

private:
    render::PModel m_spModel;  //������
    render::PFont  m_spFont;   //�����
    math::PCamera  m_spCamera; //������

    math::CTrack m_track; //����

    //�������� �����
    input::CKeyUp        m_cEsc;   //�����
    input::CKeyDown      m_cR;     //�������� ��������� � ��������� ���������
    input::CButton       m_cQ;     //������� ������ �������
    input::CButton       m_cE;     //������� �� �������
    input::CButton       m_cW;     //����������
    input::CButton       m_cS;     //�������
    input::CRelativeAxis m_cXAxis; //�������������� ��� ��������
    input::CRelativeAxis m_cYAxis; //������������ ��� ��������

    //���������� ������
    math::PTargetCamera m_spTargetCamera; //���������� ������
};