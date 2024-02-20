#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QSharedPointer>

class Q3DModel;

class ModelWindow : public QOpenGLWidget, public QOpenGLExtraFunctions
{
public:
    ModelWindow(const QString &filePath, QWidget *parent = nullptr);
    ~ModelWindow() override;

    void setRotation(float m_xRotation, float m_yRotation, float m_zRotation);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initShaders();

private:
    QOpenGLShaderProgram m_program;
    QMatrix4x4 m_model, m_view, m_project;

    QVector2D m_mousePressPosition;
    QVector3D m_rotationAxis;
    float m_angularSpeed = 0;

    QQuaternion m_rotation;

    const float c_near = 0.1f;
    const float c_far = 100.0f;
    qreal m_elapsed = 100;
    qreal m_yaw = -90;
    qreal m_pitch = 0;
    qreal m_aspect = 45;

    QVector3D m_cameraPos = {0.0f, 0.0f, 10.0f};
    QVector3D m_cameraFront = {0.0f, 0.0f, -1.0f};
    QVector3D m_cameraUp = {0.0f, 1.0f, 0.0f};
    QPoint m_last;
    bool m_keys[1024] = {false};

    QSharedPointer<Q3DModel> ourModel = Q_NULLPTR;
    QString m_filePath;
};

#endif // MODELWINDOW_H

