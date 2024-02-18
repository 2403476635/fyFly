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
    glClearColor(10.05f, 0.05f, 0.05f, 1.0f);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkKey();
    //qInfo() << "===========paintGL:" << m_cameraPos << m_cameraFront << m_cameraUp;
    // view/projection transformations
    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    m_project.setToIdentity();
    m_project.perspective(m_aspect, width() / height(), c_near, c_far);

    QMatrix4x4 matrix;
    matrix.translate(0.0f, 0.0f, 0.0f);
    matrix.scale(0.05f);
    matrix.rotate(m_rotation);
    m_model = matrix;

    m_program.setUniformValue("projection", m_project);
    m_program.setUniformValue("view", m_view);
    m_program.setUniformValue("model", m_model);

    if (!ourModel.isNull())
        ourModel->drawModel(&m_program);
}

void ModelWindow::timerEvent(QTimerEvent *)
{
//    m_angularSpeed *= 0.99f;
//    if (m_angularSpeed <= 0.01f) {
//        m_angularSpeed = 0;
//    } else {
//        m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, m_angularSpeed) * m_rotation;
//        update();
//    }
}

void ModelWindow::mousePressEvent(QMouseEvent *event)
{
    m_last = event->pos();
    m_mousePressPosition = QVector2D{event->position()};
}

void ModelWindow::mouseMoveEvent(QMouseEvent *event)
{
//    QPointF diff = event->pos() - m_last;
//    m_last = event->pos();

//    qreal sensitivity = 0.05;
//    qreal xOffset = diff.x() * sensitivity;
//    qreal yOffset = diff.y() * sensitivity;
//    m_yaw += xOffset;
//    m_pitch += yOffset;
//    if (m_pitch > 89.0) {
//        m_pitch = 89.0;
//    } else if (m_pitch < -89.0) {
//        m_pitch = -89.0;
//    }
//    QVector3D front;
//    front.setX(cos(qDegreesToRadians(m_pitch)) * cos(qDegreesToRadians(m_yaw)));
//    front.setY(sin(qDegreesToRadians(m_pitch)));
//    front.setZ(cos(qDegreesToRadians(m_pitch)) * sin(qDegreesToRadians(m_yaw)));
//    m_cameraFront = front.normalized();

    QVector2D diff2d = (QVector2D)event->pos() - (QVector2D)m_last;
    qreal angle = (diff2d.length())/100.0f;
    QVector3D n = QVector3D(diff2d.y(), diff2d.x(), 0.0).normalized();
    m_rotation = QQuaternion::fromAxisAndAngle(n, angle) * m_rotation;

    update();

    event->accept();
}
void ModelWindow::wheelEvent(QWheelEvent *event)
{
    int offset = event->angleDelta().y()< 0 ? -1 : 1;
    qreal speed = 10;
    if (1<= m_aspect + offset * speed && m_aspect + offset * speed <= 45) {
        m_aspect = m_aspect + offset * speed;
    }
    event->accept();
}

void ModelWindow::mouseReleaseEvent(QMouseEvent *event)
{
//    QVector2D diff = QVector2D{event->position()} - m_mousePressPosition;
//    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

//    float acc = diff.length() / 100.0f;
//    m_rotationAxis = (m_rotationAxis * m_angularSpeed + n *acc).normalized();

//    m_angularSpeed += acc;
    event->accept();
}

void ModelWindow::keyPressEvent(QKeyEvent *event)
{
    if (0 <= event->key() && event->key() < (int)(sizeof(m_keys)/ sizeof(m_keys[0]))) {
        m_keys[event->key()] = true;
    }
    if (event->key() == Qt::Key_F5) {
        m_model.setToIdentity();
        m_view.setToIdentity();
        m_project.setToIdentity();

        m_cameraPos  = QVector3D(0.0f, 0.0f, 10.0f);
        m_cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
        m_cameraUp  = QVector3D(0.0f, 1.0f, 0.0f);
        m_elapsed = 100;
        m_yaw = -90;
        m_pitch = 0;
        m_aspect = 45;
        m_rotation = QQuaternion();
        qInfo() << "================Qt::Key_F5";
        update();
    }
    event->accept();
}

void ModelWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (0 <= event->key() && event->key() < (int)(sizeof(m_keys)/ sizeof(m_keys[0]))) {
        m_keys[event->key()] = false;
    }
    event->accept();
}

void ModelWindow::checkKey()
{
    GLfloat cameraSpeed  = 0.05 * m_elapsed/ 1000.0;
    if (m_keys[Qt::Key_A]) {
        m_cameraPos -= QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
        qDebug() << "key a, left";
        update();
    }
    if (m_keys[Qt::Key_D])  {
        m_cameraPos += QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
        qDebug() << "key d, right";
        update();
    }
    if(m_keys[Qt::Key_W]) {
        m_cameraPos += cameraSpeed * m_cameraFront;
        qDebug() << "key w, front";
        update();
    }
    if (m_keys[Qt::Key_S])  {
        m_cameraPos -= cameraSpeed * m_cameraFront;
        qDebug() << "key s, back";
        update();
    }
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
