#include "model.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Model* Model::m;

Model::Model(){}

void Model::Draw(Shader shader){


    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);

}

void Model::unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    /*
     * The idea of this algorithm is:
     * we iterate from 0 to the size of the vector and compare each other
     * if we find two that are equal, we substitute that one in the EBO
     * the "continue" part is in case we try to check the same vertix again.
     * the i-count is because the vertex 300 in the old scheme is going to become 300-count where count is the number of vertices that were taken out
   */
    std::vector <unsigned int> newInd;
    std::vector <Vertex> newVs;
    std::vector <unsigned int> repeatedVs;

    newInd = ind;
    for(unsigned int i = 0; i<vs.size(); i++){
        if(std::find(repeatedVs.begin(), repeatedVs.end(), i) != repeatedVs.end() ){
            continue;
        }
        newVs.push_back(vs[i]);
        newInd[i] = newVs.size()-1;

        for(unsigned int j=i+1; j<vs.size(); j++){
            // Apparently there are vertices with the same position but different normal values
            // Since the code wants vertex at same position (and since we can calculate normals in the geometry shader)
            // I am going to stick with the "same vertex position" approach
                if(vs[i].Pos == vs[j].Pos /*&& vs[i].Normal == vs[j].Normal*/){
                    repeatedVs.push_back(j);
                    newInd[j] = newVs.size()-1;
                }
        }
    }

    vs = newVs;
    ind = newInd;

}
std::vector<std::pair<unsigned int, unsigned int> > Model::PairSelection(std::vector<unsigned int> &ind){

    std::vector<std::pair<unsigned int, unsigned int> > pairs;
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
    unsigned k;
    for(k = 0; k<ind.size(); k++){
        v1 = ind[k++]; v2 = ind[k++]; v3=ind[k];

        if(std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v1, v2)) != pairs.end() ||
                std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v2, v1)) != pairs.end() ){
        } else {
         pairs.push_back(std::pair<unsigned int, unsigned int>(v1,v2));
        }
        if(std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v1, v3)) != pairs.end() ||
            std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v3, v1)) != pairs.end()){
        } else {
            pairs.push_back(std::pair<unsigned int, unsigned int>(v1,v3));
        }
        if(std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v3, v2)) != pairs.end() ||
            std::find(pairs.begin(), pairs.end(),std::pair<unsigned int, unsigned int>(v2, v3)) != pairs.end() ){
        } else{
            pairs.push_back(std::pair<unsigned int, unsigned int>(v2,v3));
        }
    }
    return pairs;
}
void Model::Optimization(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){


        unRepVert(vs, ind);
        std::vector<glm::mat4> ListQ = calcQMatrices(vs, ind);
        std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection(ind);
        std::vector<Pair> Pairs = formPairList(vs,pairs, ListQ);
        // Note that number of pairs has to satisfy Faces + Vertices - Edges = 2 (if it is a Polyhedra)
        // Where faces = ind.size()/3 and vertices is just vs.size()
        std::cout << "success" << std::endl;

        while((ind.size()/3)>1000){
            changeVert2(Pairs, vs, ind, ListQ);
            //Notice that after that, the number of Pairs should go down by at least 3
            //However, I am not taking into account the Pairs that are repeated
            //There are two Pairs that are repeated for every iteration
            //I can't be bothered to take care of that now, but this needs to be fixed
             std::sort(Pairs.begin(), Pairs.end(), less_than_cost());
        }
        std::cout<< "success" << std::endl;
}

void Model::changeVert2(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ){
        /*
         * The idea of this function is to update the variables Pairs, vs, ind and ListQs
         *  1-> Assign the vertex with the lowest index as v_dash
         *  2-> Assign the vertex with the highest index as v_remove
         *  3-> Update vs to make vs[v_dash] = contVert
         *  4-> Update Qs to make Q[v_dash] = v_dashQ
         *  5-> Remove the first Pair from the Pairs list
         *  6-> Change all of the Pairs from v_remove/v_dash to v_dash, which involves
         *      6.1 Changing its vecPair value from <v1,v2> to <v1,v_dash>
         *      6.2 Changing its Q value to Q of v_dash + Q of its pair
         *      6.3 Changing its contVert.Pos value to (v_dash.Pos+itsPair.Pos)/2
         *      6.4 Changing its contVert.Normal value to normalize(v_dash.Normal+itsPair.Normal)
         *      6.5 Changing its cost value to contVert*Q*contVert
         *  7-> For all values of Ind
         *        7.1 Change from v_remove to v_dash
         *        7.2 Find if this face has two v_dash and add the different one to a variable "repeated"
         *        7.3 Remove face that has two v_dash
         *        This repeated values are the vertex numbers that has duplicated edge with v_dash in the Pairs variable
         *  8-> Remove Pairs that are duplicated according to the variable "repeated"
         *  9-> Erase vs[v_remove]
         *  10-> Erase listQ[v_remove]
         *  11-> Iterate through Pairs and ind subtracting one from all values that are bigger than v_remove
         *       Remember that there are no vertices with index == v_remove since we changed them to v_dash
         * */

        // 1 and 2
        unsigned int v_dash;
        unsigned int v_remove;
        std::vector<unsigned int> repeated;
        if(Pairs[0].vecPair.first > Pairs[0].vecPair.second){
            v_remove = Pairs[0].vecPair.first;
            v_dash = Pairs[0].vecPair.second;
        } else {
            v_remove = Pairs[0].vecPair.second;
            v_dash = Pairs[0].vecPair.first;
        }

        // 3
        vs[v_dash] = Pairs[0].contVert;
        // 4
        listQ[v_dash] = Pairs[0].Q;
        // 5
        Pairs.erase(Pairs.begin());
        // 6
        for(unsigned int w=0; w<Pairs.size(); w++){
            unsigned int theOther;
            if(!(Pairs[w].vecPair.first==v_remove||Pairs[w].vecPair.first==v_dash||Pairs[w].vecPair.second==v_remove||Pairs[w].vecPair.second==v_dash))
                continue;

            // 6.1
            if(Pairs[w].vecPair.first == v_remove||Pairs[w].vecPair.first == v_dash){
                Pairs[w].vecPair.first = v_dash;
                theOther = Pairs[w].vecPair.second;
            }
            if(Pairs[w].vecPair.second==v_remove||Pairs[w].vecPair.second==v_dash){
                Pairs[w].vecPair.second = v_dash;
                theOther = Pairs[w].vecPair.first;
            }

            //6.2
            Pairs[w].Q = listQ[theOther] + listQ[v_dash];

            //6.3
            Pairs[w].contVert.Pos = vs[v_dash].Pos+vs[theOther].Pos;
            Pairs[w].contVert.Pos = glm::vec3(Pairs[w].contVert.Pos.x/2.0f, Pairs[w].contVert.Pos.y/2.0f, Pairs[w].contVert.Pos.z/2.0f);

            //6.4
            Pairs[w].contVert.Normal = glm::normalize(vs[v_dash].Normal+vs[theOther].Normal);

            //6.5
            glm::vec4 c = Pairs[w].Q*glm::vec4(Pairs[w].contVert.Pos,1.0f);
            Pairs[w].cost = glm::dot(c, glm::vec4(Pairs[w].contVert.Pos,1.0f));

        }

        //7
        int nFacesRemoved=0;
        for(unsigned int w=0; w<ind.size(); w++){
            unsigned int v1 = ind[w++];
            unsigned int v2 = ind[w++];
            unsigned int v3 = ind[w];

            //7.1
            if(v1==v_remove)
                v1=v_dash;
            if(v2==v_remove)
                v2=v_dash;
            if(v3==v_remove)
                v3=v_dash;

            //7.2
            if(v1==v2)
                repeated.push_back(v3);
            if(v1==v3)
                repeated.push_back(v2);
            if(v2==v3)
                repeated.push_back(v1);

            //7.3
            if(v1==v2||v1==v3||v2==v3){
                ind.erase(ind.begin() + w-2, ind.begin() + w+1);
                nFacesRemoved++;
                if(nFacesRemoved==3){
                    std::cout<<"Something is wrong: 3 faces removed" <<std::endl;
                }
                w-=3;
            }

        }

        //8
        for(unsigned int w=0; w<Pairs.size();w++){
            if(repeated.size()==0)
                break;
            for(unsigned int i=0; i<repeated.size(); i++){
                if((Pairs[w].vecPair.first == repeated[i] && Pairs[w].vecPair.second==v_dash) ||
                    (Pairs[w].vecPair.first == v_dash && Pairs[w].vecPair.second==repeated[i])){
                    Pairs.erase(Pairs.begin()+w);
                    repeated.erase(repeated.begin()+i);
                    --w;
                    continue;
                }

            }
        }

        //9
        vs.erase(vs.begin()+v_remove);

        //10
        listQ.erase((listQ.begin() +v_remove));

        //11
        for(unsigned int w = 0; w<Pairs.size(); w++){
            if(Pairs[w].vecPair.first>v_remove)
                Pairs[w].vecPair.first--;
            if(Pairs[w].vecPair.second>v_remove)
                Pairs[w].vecPair.second--;
        }
        for(unsigned int w=0; w<ind.size(); w++){
            if(ind[w]>v_remove)
                ind[w]--;
        }

}

void Model::changeVert(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ){
        unsigned int v1 = Pairs[0].vecPair.first;
        unsigned int v2 = Pairs[0].vecPair.second;
        unsigned int v_dashInd;
        unsigned int v_remove;
        std::vector<unsigned int> commonVs;
        Vertex v_dash = Pairs[0].contVert;
        glm::mat4 v_dashQ = Pairs[0].Q;
        // Select vertex with smallest index in ind and change it to v_dash
        if(v1>v2){
            v_dashInd = v2;
            v_remove = v1;
        }
        else{
            v_dashInd = v1;
            v_remove = v2;
        }
        vs[v_dashInd] = v_dash;

        // Remove the vertex with the biggest index in vs
        vs.erase(vs.begin() + v_remove);
        // Update listQ
        listQ.erase(listQ.begin() + v_remove);
        listQ[v_dashInd] = v_dashQ;

        // Change all the indeces of the ind (they all displaced by one) in the ind and remove the face that had both v1 and v2
        for(unsigned int w = 0; w<ind.size(); w+=3){
            unsigned int ind1 = ind[w];
            unsigned int ind2 = ind[w+1];
            unsigned int ind3 = ind[w+2];

            // Delete Faces that had both vertices as an edge
            if((v1==ind1||v1==ind2||v1==ind3)&&(v2==ind1||v2==ind2||v2==ind3)){
                ind.erase(ind.begin() + w, ind.begin() + w+3);
                if(ind1!=v1 && ind1 != v2)
                    commonVs.push_back(ind1);
                if(ind2!=v1 && ind2 != v2)
                    commonVs.push_back(ind2);
                if(ind3!=v1 && ind3 != v2)
                    commonVs.push_back(ind3);
                w-=3;
                continue;
            }

            if(ind1>=v_remove){
                if(ind1 == v_remove){
                    ind[w] =v_dashInd;
                } else{
                    ind[w] = ind[w] -1;
                }
            }
            if(ind2>=v_remove){
                if(ind2 == v_remove){
                    ind[w+1] =v_dashInd;
                } else{
                    ind[w+1] = ind[w+1] -1;
                }
            }
            if(ind3>=v_remove){
                if(ind3== v_remove){
                    ind[w+2] =v_dashInd;
                } else{
                    ind[w+2] = ind[w+2] -1;
                }
            }
        }
        for(int k =0; k<commonVs.size(); k++){
            if(commonVs[k]>v_remove)
                commonVs[k]--;
        }

        // Update Pairs Vector
//        Pairs.erase(Pairs.begin());
//        for(unsigned int w=0; w<Pairs.size(); w++){
//            // From now on, the Pairs can have 4 cases:
//            // 1. It has the vertex that was erased as first element
//            // 2. It has the vertex that was erased as second element
//            // 3. It has the vertex that was substituted as first element
//            // 4. It has the vertex that was substituted as second element
//            // The update for the first and third case is the same
//            // The update for the second and fourth case is the same
//            unsigned int fv = Pairs[w].vecPair.first;
//            unsigned int sv = Pairs[w].vecPair.second;
//            // Case 1:
//            if(fv == v_remove||fv== v_dashInd){
//                Pair p;
//                std::pair<unsigned int, unsigned int> k(v_dashInd, sv);
//                if(sv>v_remove) // If the second vertex is bigger than v_remove, update it
//                    --sv;
//                p.vecPair = k;
//                p.contVert.Pos = vs[v_dashInd].Pos + vs[sv].Pos;
//                p.contVert.Pos = glm::vec3(p.contVert.Pos.x/2 , p.contVert.Pos.y/2, p.contVert.Pos.z/2);
//                p.contVert.Normal = vs[v_dashInd].Normal + vs[sv].Normal;
//                p.contVert.Normal = glm::normalize(p.contVert.Normal);
//                p.Q = listQ[sv] + v_dashQ;
//                //This is a v^t*Q*v Operation
//                glm::vec4 Cv = p.Q*glm::vec4(p.contVert.Pos,1.0f);
//                long double cost = glm::dot(glm::vec4(p.contVert.Pos,1.0f),Cv);
//                p.cost = cost;
//                Pairs[w] = p;
//                continue;
//            }
//            // Case 2:
//            if(sv == v_remove || sv == v_dashInd){
//                Pair p;
//                std::pair<unsigned int, unsigned int> k(v_dashInd, fv);
//                if(fv>v_remove) // If the second vertex is bigger than v_remove, update it
//                    --fv;
//                p.vecPair = k;
//                p.contVert.Pos = vs[v_dashInd].Pos + vs[fv].Pos;
//                p.contVert.Pos = glm::vec3(p.contVert.Pos.x/2 , p.contVert.Pos.y/2, p.contVert.Pos.z/2);
//                p.contVert.Normal = vs[v_dashInd].Normal + vs[sv].Normal;
//                p.contVert.Normal = glm::normalize(p.contVert.Normal);
//                p.Q = listQ[fv] + v_dashQ;

//                //This is a v^t*Q*v Operation
//                glm::vec4 Cv = p.Q*glm::vec4(p.contVert.Pos,1.0f);
//                long double cost = glm::dot(glm::vec4(p.contVert.Pos,1.0f),Cv);
//                p.cost = cost;
//                Pairs[w] = p;
//                continue;
//            }
//            if(sv>v_remove){
//                Pairs[w].vecPair.second--;
//            }
//            if(fv>v_remove){
//                Pairs[w].vecPair.first--;
//            }
//        }

//        for(unsigned int w=0; w<Pairs.size(); w++){
//            unsigned int first = Pairs[w].vecPair.first;
//            unsigned int second = Pairs[w].vecPair.second;
//            for(unsigned int k = 0; k<commonVs.size(); k++){
//                if((first==v_dashInd||second==v_dashInd)&&(first==commonVs[k]||second==commonVs[k])){
//                    commonVs.erase(commonVs.begin()+k);
//                    Pairs.erase(Pairs.begin() + w);
//                    --w;
//                    continue;
//                }
//            }
//            if(commonVs.size() ==0){
//                break;
//            }
//        }
//        for(unsigned int w=0; w<Pairs.size(); w++){
//            for(unsigned int k = w+1; k<Pairs.size(); k++){
//                if(Pairs[w].vecPair == Pairs[k].vecPair){
//                    Pairs.erase(Pairs.begin()+k);
//                    --k;
//                }
//            }
//        }
        // Check when pairs have the same "face"

        // Worst way of doing it, but at least it works
        std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection(ind);
        Pairs = formPairList(vs,pairs, listQ);

}

std::vector<Pair> Model::formPairList(std::vector<Vertex> &vs, std::vector<std::pair<unsigned int, unsigned int> > &pairs, std::vector<glm::mat4> &Q){

    std::vector<Pair> Pairs;
    for(unsigned int k= 0; k<pairs.size(); k++){
        Pair p;
        p.vecPair = pairs[k];
        p.Q = Q[p.vecPair.first]+Q[p.vecPair.second];
        //if invertible
        /* SOLVE THE MINIMUM SQUARE METHOD
         * Not working at the moment and I cannot be bothered to figure out why not
         * Postpone it a bit
         * */
//        const float *y = (const float*)glm::value_ptr(p.Q);
//        float l[16] = {y[0], y[1], y[2],y[3], y[4], y[5], y[6], y[7], y[8],y[9], y[10], y[11],  y[12],y[13], y[14], y[15]};
//        glm::mat4 Opt = glm::make_mat4(l);
//        glm::vec4 v_dash = glm::inverse(Opt)*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//        v_dash = glm::vec4(v_dash.x/v_dash.w, v_dash.y/v_dash.w, v_dash.z/v_dash.w, v_dash.w/v_dash.w);
        p.contVert.Pos = (vs[p.vecPair.first].Pos + vs[p.vecPair.second].Pos);
        p.contVert.Pos = glm::vec3(p.contVert.Pos.x/2, p.contVert.Pos.y/2, p.contVert.Pos.z/2);
        p.contVert.Normal = (vs[p.vecPair.first].Normal + vs[p.vecPair.second].Normal);
        p.contVert.Normal = glm::normalize(p.contVert.Normal);

        glm::vec4 m = glm::vec4(p.contVert.Pos, 1.0f);
        glm::vec4 pl = m*glm::mat4(p.Q);
        long double cost = glm::dot(pl, glm::vec4(p.contVert.Pos, 1.0f));
        p.cost = cost;

        Pairs.push_back(p);
    }
    std::sort(Pairs.begin(), Pairs.end(), less_than_cost());

    return Pairs;
}

std::vector<glm::mat4> Model::calcQMatrices(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

    std::vector<glm::vec4> P;
    std::vector<glm::mat4> ListKp;

    // Calculate the planes that define each face in the form vec4 (a,b,c,d) so that
    // ax + by + cz + d = 0, which defines a plane in (x,y,z)
    for(unsigned int k = 0; k<ind.size(); k++){
        // Calculate P for each plane (set of three vertices)
        glm::vec3 v1 = vs[ind[k++]].Pos;
        glm::vec3 v2 = vs[ind[k++]].Pos;
        glm::vec3 v3 = vs[ind[k]].Pos;
        // Notice that ax + by + cz + d = 0 for p = <a,b,c> is such that <a,b,c> is the vector normal to the plane
        glm::vec3 normal = glm::normalize(glm::cross(v1-v3,v2-v3)); // this is <a,b,c>
        float d = -glm::dot(v3, normal);
        P.push_back(glm::vec4(normal, d));

    }

    // Find all planes that the vertex belongs to and calculate Q for each
    for(unsigned int k = 0; k<vs.size(); k++){
        // Find all planes for Vertex k
        unsigned int i = 0;
        std::vector<glm::vec4> Planes;
        while (i < ind.size())
        {
            if(k == ind[i]){
                // Ever 3 points define a face, so if we devide by 3
                // The face number will be the integer part
                // ex if i = 16, i/3 = 5.33333, so it is the face number 5;
                unsigned int w = floor(i/3);
                Planes.push_back(P[w]);
            }
            i++;
        }
        // Sum all Kp for that Vertex to get Q
        glm::mat4 Kp;
        bool first = true;
        for(unsigned t =0; t<Planes.size(); t++){
            if(first){
                Kp = glm::outerProduct(Planes[t], Planes[t]);
            }else{
                Kp = Kp + glm::outerProduct(Planes[t], Planes[t]);
            }
            first = false;
        }
        ListKp.push_back(Kp);
    }
    return ListKp;
}

void Model::loadModel(std::string path){
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureS> textures;

    // Walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Pos = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // tangent
        if(mesh->mTangents != NULL){
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
        }
        // bitangent
        if(mesh->mBitangents !=NULL){
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
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
    std::vector<TextureS> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<TextureS> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<TextureS> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<TextureS> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data

    /* Notice that when the File is .stl, there will be loads of repeated vertices.
     * This is because stl doesn't use EBOs
     * So the next function tries to rectify this by assigning the right indices to the right vertices.
     * **/
    TextureEngine::SurfaceSimplificationEngine::Optimize(vertices, indices);

    return Mesh(vertices, indices, textures);
}

std::vector<TextureS> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
   {
       std::vector<TextureS> textures;
       for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
       {
           aiString str;
           mat->GetTexture(type, i, &str);
           // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
           bool skip = false;
           for(unsigned int j = 0; j < textures_loaded.size(); j++)
           {
               if(std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
               {
                   textures.push_back(textures_loaded[j]);
                   skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                   break;
               }
           }
           if(!skip)
           {   // if texture hasn't been loaded already, load it
               TextureS texture;
               texture.id = TextureFromFile(str.C_Str(), this->directory);
               texture.type = typeName;
               texture.path = str;
               textures.push_back(texture);
               textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
           }
       }
       return textures;
   }


unsigned int Model::TextureFromFile(const char *path, const std::string &directory)
{
    glewInit();
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
