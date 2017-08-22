#include "textureengine.h"

void TextureEngine::SurfaceSimplificationEngine::Optimize(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    unRepVert(vs, ind);
    std::vector<glm::mat4> ListQ = calcQMatrices(vs, ind);
    std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection(ind);
    std::vector<Pair> Pairs = formPairList(vs,pairs, ListQ);
    // Note that number of pairs has to satisfy Faces + Vertices - Edges = 2 (if it is a Polyhedra)
    // Where faces = ind.size()/3 and vertices is just vs.size()

    while(vs.size()>30){
        changeVert(Pairs, vs, ind, ListQ);
        //Notice that after that, the number of Pairs should go down by at least 3 (if Vertex is not an edge)
    }

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
void TextureEngine::SurfaceSimplificationEngine::changeVert(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ){
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
         *  12-> Sort Pairs in order of cost in preparation for the next iteration
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
        for(unsigned int w=0; w<ind.size(); w++){
            unsigned int *v1 = &ind[w++];
            unsigned int *v2 = &ind[w++];
            unsigned int *v3 = &ind[w];

            //7.1
            if(*v1==v_remove)
                *v1=v_dash;
            if(*v2==v_remove)
                *v2 = v_dash;
            if(*v3==v_remove)
                *v3=v_dash;

            //7.2
            if(*v1==*v2)
                repeated.push_back(*v3);
            if(*v1==*v3)
                repeated.push_back(*v2);
            if(*v2==*v3)
                repeated.push_back(*v1);


            if(*v1==*v2||*v1==*v3||*v2==*v3){
                ind.erase(ind.begin() + w-2, ind.begin() + w+1);
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

        //12
        std::sort(Pairs.begin(), Pairs.end(), less_than_cost());
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
