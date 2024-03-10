#include "ModelWindow.h"
#include "Q3DModel.h"

#include <QDebug>
#include <QMouseEvent>
#include <QtMath>

ModelWindow::ModelWindow(const QString &filePath, QWidget *parent) : QOpenGLWidget(parent)
{
    m_filePath = filePath;
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

ModelWindow::~ModelWindow()
{
    makeCurrent();
    ourModel = Q_NULLPTR;
    doneCurrent();
}

void ModelWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 将背景颜色设为白色
    initShaders();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    qInfo() << "initializeGL";
    ourModel = QSharedPointer<Q3DModel>(new Q3DModel(m_filePath));

    startTimer(1000 / 60);
}

void ModelWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    float ratio = float(w) / (h ? h : 1);
    m_project.setToIdentity();
    m_project.perspective(m_aspect, ratio, c_near, c_far);
}

void ModelWindow::paintGL()
{
    // 清空颜色缓冲和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // view/projection transformations
    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    m_project.setToIdentity();
    m_project.perspective(m_aspect, width() / height(), c_near, c_far);

    QMatrix4x4 matrix;
    matrix.translate(0.0f, -0.5f, -5.0f); /* 水平移动 */
    matrix.scale(0.005f);               /* 缩放 */
    matrix.rotate(m_rotation);
    m_model = matrix;

    m_program.setUniformValue("projection", m_project);
    m_program.setUniformValue("view", m_view);
    m_program.setUniformValue("model", m_model);

    if (!ourModel.isNull())
        ourModel->drawModel(&m_program);


}

void ModelWindow::initShaders()
{
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/vshader.glsl")) {
        qDebug() << __FILE__<<__FUNCTION__<< " add vertex shader file failed.";
        close();
    }
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/fshader.glsl")) {
        qDebug() << __FILE__<<__FUNCTION__<< " add fragment shader file failed.";
        close();
    }

    if (!m_program.link()) {
        qDebug() << __FILE__<<__LINE__<< "program link failed";
        close();
    }
    if (!m_program.bind()) {
        qDebug() << __FILE__<<__LINE__<< "program bind failed";
        close();
    }
}

void ModelWindow::setRotation(float m_xRotation, float m_yRotation, float m_zRotation)
{
    m_rotation = QQuaternion::fromEulerAngles(m_xRotation, m_yRotation, m_zRotation);
    update();
}
