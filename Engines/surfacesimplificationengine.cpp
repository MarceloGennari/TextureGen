#include "textureengine.h"

void TextureEngine::SurfaceSimplificationEngine::Optimize(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    unRepVert(vs, ind);
    std::vector<glm::mat4> ListQ = calcQMatrices(vs, ind);
    std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection(ind);
    std::vector<Pair> Pairs = formPairList(vs,pairs, ListQ);
    // Note that number of pairs has to satisfy Faces + Vertices - Edges = 2 (if it is a Polyhedra)
    // Where faces = ind.size()/3 and vertices is just vs.size()
    std::cout << "success" << std::endl;

    while(vs.size()>2700){
        changeVert(Pairs, vs, ind, ListQ);
        //Notice that after that, the number of Pairs should go down by at least 3
        //However, I am not taking into account the Pairs that are repeated
        //There are two Pairs that are repeated for every iteration
        //I can't be bothered to take care of that now, but this needs to be fixed
         std::sort(Pairs.begin(), Pairs.end(), less_than_cost());
    }
    std::cout<< "success" << std::endl;
}

void TextureEngine::SurfaceSimplificationEngine::unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    /*
     * The idea of this algorithm is:
     * we iterate from 0 to the size of the vector and compare each other
     * if we find two that are equal, we substitute that one in the EBO
     * the "continue" part is in case we try to check the same vertex again.
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
            // Since the code wants vertex at same position (and since we can calculate normals afterwards)
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
std::vector<std::pair<unsigned int, unsigned int> > TextureEngine::SurfaceSimplificationEngine::PairSelection(std::vector<unsigned int> &ind){
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
void TextureEngine::SurfaceSimplificationEngine::changeVert(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ){
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
    //                if(first == commonVs[k]){
    //                    commonVs.erase(commonVs.begin()+k);
    //                    Pairs.erase(Pairs.begin() + w);
    //                }
    //                if(second == commonVs[k]){
    //                    commonVs.erase(commonVs.begin()+k);
    //                    Pairs.erase(Pairs.begin() + w);
    //                }
    //            }
    //            if(commonVs.size() ==0){
    //                break;
    //            }
    //        }
        // Check when pairs have the same "face"


        std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection(ind);
        Pairs = formPairList(vs,pairs, listQ);
}

std::vector<Pair> TextureEngine::SurfaceSimplificationEngine::formPairList(std::vector<Vertex> &vs, std::vector<std::pair<unsigned int, unsigned int> > &pairs, std::vector<glm::mat4> &Q){

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

std::vector<glm::mat4> TextureEngine::SurfaceSimplificationEngine::calcQMatrices(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

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
