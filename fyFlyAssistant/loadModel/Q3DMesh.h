#ifndef Q3DMESH_H
#define Q3DMESH_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QVector2D>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    QVector3D Position;
    // normal
    QVector3D Normal;
    // texCoords
    QVector2D TexCoords;
    // tangent
    QVector3D Tangent;
    // bitangent
    QVector3D Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    uint id;
    QString type;
    QString path;
};

class Q3DMesh : public QOpenGLExtraFunctions
{
public:
    // constructor
    Q3DMesh(QVector<Vertex> vertices, QVector<uint> indices, QVector<Texture> textures);
    // render the mesh
    void drawMesh(QOpenGLShaderProgram *shader);

private:
    // initializes all the buffer objects/arrays
    void setupMesh();

private:
    // mesh Data
    QVector<Vertex> vertices;
    QVector<uint> indices;
    QVector<Texture> textures;
    uint VAO;
    // render data
    uint VBO, EBO;
};
#endif
