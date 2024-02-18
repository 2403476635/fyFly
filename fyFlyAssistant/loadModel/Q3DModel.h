#ifndef Q3DMODEL_H
#define Q3DMODEL_H

#include "Q3DMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

class Q3DModel : public QOpenGLExtraFunctions
{
public:
    // constructor, expects a filepath to a 3D model.
    Q3DModel(const QString &path, bool gamma = false);
    ~Q3DModel();

    // draws the model, and thus all its meshes
    void drawModel(QOpenGLShaderProgram *shader);
    
private:
    uint textureFromFile(const QString &path, const QString &directory, bool gamma);

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const QString &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Q3DMesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    QVector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName);

private:
    // model data
    QVector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    QVector<Q3DMesh> meshes;
    QString directory;
    bool gammaCorrection;
};
#endif
