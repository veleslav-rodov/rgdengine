#include "precompiled.h"

#include <rgde/math/fly_camera.h>

namespace math
{

    free_camera::free_camera(camera_ptr camera)
    {
        set_camera(camera);

        base::lmsg << "free_camera::free_camera()";
        m_up       = vec3f(0.0f, 1.0f, 0.0f);
        m_eye_pos    = vec3f(0.0f, 0.0f, 0.0f);
        m_lookat_pt = vec3f(0.0f, 0.0f, 1.0f);
        do_ortho_normal();
    }

    free_camera_ptr free_camera::create(camera_ptr camera)
    {
        return free_camera_ptr(new free_camera(camera));
    }

    void free_camera::set_position(const vec3f& up, const vec3f& eye, const vec3f& look_at)
    {
        m_up       = up;
        m_eye_pos    = eye;
        m_lookat_pt = look_at;
        apply();
    }

    void free_camera::get_pos(vec3f& up, vec3f& eye, vec3f& look_at)
    {
        up       = m_up;
        eye    = m_eye_pos;
        look_at = m_lookat_pt;
    }

    void free_camera::move_forward(float delta)
    {
        vec3f vDelta = m_lookat_pt-m_eye_pos;
        normalize(vDelta);
        vDelta *= delta;

        m_eye_pos    += vDelta;
        m_lookat_pt += vDelta;

        apply();
    }

    void free_camera::move_left(float delta)
    {
        vec3f vDelta;
        cross(vDelta, m_up, vec3f(m_lookat_pt-m_eye_pos));
        normalize(vDelta);
        vDelta *= delta;

        m_eye_pos    -= vDelta;
        m_lookat_pt -= vDelta;

        apply();
    }

    void free_camera::move_up(float delta)
    {
        vec3f vDelta = m_up;
        normalize(vDelta);
        vDelta *= delta;

        m_eye_pos    += vDelta;
        m_lookat_pt += vDelta;

        apply();
    }

    void free_camera::rotate_right(float angle)
    {
        quatf rot;
        vec3f vAxis = m_up;

        normalize(vAxis);
        setRot(rot, AxisAnglef(angle, vAxis));

        m_lookat_pt = xform<float>(m_lookat_pt, rot, m_lookat_pt-m_eye_pos) + m_eye_pos;
        apply();
    }

    void free_camera::rotate_up(float angle)
    {
        quatf rot;
        vec3f vAxis;

        cross(vAxis, m_up, vec3f(m_lookat_pt-m_eye_pos));
        normalize(vAxis);
        setRot(rot, AxisAnglef(angle, vAxis));

        m_lookat_pt = xform<float>(m_lookat_pt, rot, m_lookat_pt-m_eye_pos) + m_eye_pos;
        m_up = xform<float>(m_up, rot, m_up);
        apply();
    }

    void free_camera::rotate_cw(float angle)
    {
        quatf rot;
        vec3f vAxis = m_lookat_pt-m_eye_pos;

        normalize(vAxis);
        setRot(rot, AxisAnglef(angle, vAxis));

        m_up = xform<float>(m_up, rot, m_up);
        apply();
    }

    void free_camera::activate()
    {
        apply();
    }

    void free_camera::apply()
    {
        do_ortho_normal();
        if (m_camera)
        {
            try{
                m_camera->look_at(m_eye_pos,m_lookat_pt,m_up);
                m_camera->activate();
            }
            catch(...){}
        }
    }

    //�������������, ��� ������� "������" � "�����" ������� �������������� � �����������
    void free_camera::do_ortho_normal()
    {
        normalize(m_up);

        vec3f vForward = m_lookat_pt-m_eye_pos;
        normalize(vForward);

        //m_up
        vec3f vTmp;
        cross(vTmp, m_up, vForward);
        cross(m_up, vForward, vTmp);

        //m_lookat_pt
        m_lookat_pt = m_eye_pos + vForward;
    }

} //namespace math