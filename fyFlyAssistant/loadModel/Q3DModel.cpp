#include "Q3DModel.h"

#include <QImage>
#include <QDebug>

Q3DModel::Q3DModel(const QString &path, bool gamma) : gammaCorrection(gamma)
{
    initializeOpenGLFunctions();
    loadModel(path);
}

Q3DModel::~Q3DModel()
{
    textures_loaded.clear();
    meshes.clear();
}

void Q3DModel::drawModel(QOpenGLShaderProgram *shader)
{
    for(int i = 0; i < meshes.size(); i++)
        meshes[i].drawMesh(shader);
}

uint Q3DModel::textureFromFile(const QString &path, const QString &directory, bool gamma)
{
    Q_UNUSED(gamma);
    QString filename = directory + '/' + path;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    QImage image = QImage(filename).convertToFormat(QImage::Format_RGBA8888);
    qInfo() << "filename: " << filename << QImage(filename);
    if (!image.isNull()) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        qInfo() << "Texture failed to load at path: " << path;
    }

    return textureID;
}

void Q3DModel::loadModel(const QString &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    qDebug() << path;
    const aiScene* scene = importer.ReadFile(path.toStdString(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        qInfo() << "ERROR::ASSIMP:: " << importer.GetErrorString();
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.mid(0, path.lastIndexOf('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Q3DModel::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Q3DMesh Q3DModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    QVector<Vertex> vertices;
    QVector<uint> indices;
    QVector<Texture> textures;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        QVector3D vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z); // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.setX(mesh->mNormals[i].x);
            vector.setY(mesh->mNormals[i].y);
            vector.setZ(mesh->mNormals[i].z);
            vertex.Normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) {// does the mesh contain texture coordinates?
            QVector2D vec(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.TexCoords = vec;
            // tangent
            vector.setX(mesh->mTangents[i].x);
            vector.setY(mesh->mTangents[i].y);
            vector.setZ(mesh->mTangents[i].z);
            vertex.Tangent = vector;
            // bitangent
            vector.setX(mesh->mBitangents[i].x);
            vector.setY(mesh->mBitangents[i].y);
            vector.setZ(mesh->mBitangents[i].z);
            vertex.Bitangent = vector;
        } else {
            vertex.TexCoords = QVector2D(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    QVector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.append(diffuseMaps);
    // 2. specular maps
    QVector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.append(specularMaps);
    // 3. normal maps
    QVector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.append(normalMaps);
    // 4. height maps
    QVector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.append(heightMaps);

    // return a mesh object created from the extracted mesh data
    return Q3DMesh(vertices, indices, textures);
}

QVector<Texture> Q3DModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName)
{
    QVector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(int j = 0; j < textures_loaded.size(); j++) {
            if(QString::compare(textures_loaded[j].path, str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip) {  // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), this->directory, gammaCorrection);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}
