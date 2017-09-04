#include "textureengine.h"


void TextureEngine::SurfaceSimplificationEngine::Optimize2(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    /*
     * This is an attempt to speed things up by moving pointers instead of values around
     * */
    bool simplify = false;

        std::vector<Vertex*> Vertices(vs.size());
        std::vector<unsigned int> Indeces(ind.size());
        unRepVert(vs, ind);

        for(unsigned int k = 0; k<vs.size(); k++){
            Vertices[k] = &vs[k];
        }

        //std::vector<glm::mat4 *> ListQ = calcQMatrices(Vertices, ind);
        //std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection2(ind);
        //std::vector<Pair *> Pairs = formPairList(Vertices,pairs, ListQ);
        // Note that number of pairs has to satisfy Faces + Vertices - Edges = 2 (if it is a Polyhedra)
        // Where faces = ind.size()/3 and vertices is just vs.size()

//        int perc = Pairs.size()*0;
//        while(Pairs.size()>perc){
//            changeVert(Pairs, Vertices, ind, ListQ);
//            //Notice that after that, the number of Pairs should go down by at least 3 (if Vertex is not an edge)
//        }

//        for(unsigned int k = 0; k<vs.size(); k++){
//            vs[k] = *Vertices[k];
//        }
}


std::vector<glm::mat4 *> TextureEngine::SurfaceSimplificationEngine::calcQMatrices(std::vector<Vertex *> &vs, std::vector<unsigned int> &ind){
    std::vector<std::vector<glm::mat4> > ListKp(vs.size());
    std::vector<glm::mat4 *> ListQ;

    // Calculate the planes that define each face in the form vec4 (a,b,c,d) so that
    // ax + by + cz + d = 0, which defines a plane in (x,y,z)
    for(unsigned int k = 0; k<ind.size(); k++){
        unsigned int v1Ind = ind[k++];
        unsigned int v2Ind = ind[k++];
        unsigned int v3Ind = ind[k];
        // Calculate P for each plane (set of three vertices)
        glm::vec3 v1 = vs[v1Ind]->Pos;
        glm::vec3 v2 = vs[v2Ind]->Pos;
        glm::vec3 v3 = vs[v3Ind]->Pos;
        // Notice that ax + by + cz + d = 0 for p = <a,b,c> is such that <a,b,c> is the vector normal to the plane
        glm::vec3 normal = glm::normalize(glm::cross(v1-v3,v2-v3)); // this is <a,b,c>
        float d = -glm::dot(v3, normal);
        glm::vec4 p = glm::vec4(normal,d);

        // Adding Kp to the three vertices
        ListKp[v1Ind].push_back(glm::outerProduct(p,p));
        ListKp[v2Ind].push_back(glm::outerProduct(p,p));
        ListKp[v3Ind].push_back(glm::outerProduct(p,p));
    }

    // Sum Kp of all vertices to find Q
    for(unsigned int k = 0; k<vs.size(); k++){
        bool first = true;
        for(unsigned i = 0; i<ListKp[k].size(); i++){
            if(first){
                ListQ.push_back(new glm::mat4);
                ListQ[k] = &ListKp[k][i];
                first = false;
            }
            else{
                glm::mat4* Q = &ListKp[k][i];
                *(ListQ[k]) = *ListQ[k]+*Q;
            }

        }

    }
    return ListQ;
}

std::vector<Pair *> TextureEngine::SurfaceSimplificationEngine::formPairList(std::vector<Vertex *> &vs, std::vector<std::pair<unsigned int, unsigned int> > &pairs, std::vector<glm::mat4 *> &Q){

    std::vector<Pair *> Pairs;
    for(unsigned int k= 0; k<pairs.size(); k++){
        Pair *p = new Pair;
        p->vecPair = pairs[k];
        glm::mat4 i = *Q[p->vecPair.first];
        p->Q = *(Q[p->vecPair.first])+*(Q[p->vecPair.second]);
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
        p->contVert.Pos = (vs[p->vecPair.first]->Pos + vs[p->vecPair.second]->Pos);
        p->contVert.Pos = glm::vec3(p->contVert.Pos.x/2, p->contVert.Pos.y/2, p->contVert.Pos.z/2);
        p->contVert.Normal = (vs[p->vecPair.first]->Normal + vs[p->vecPair.second]->Normal);
        p->contVert.Normal = glm::normalize(p->contVert.Normal);

        glm::vec4 m = glm::vec4(p->contVert.Pos, 1.0f);
        glm::vec4 pl = m*glm::mat4(p->Q);
        long double cost = glm::dot(pl, glm::vec4(p->contVert.Pos, 1.0f));
        p->cost = cost;

        Pairs.push_back(p);
    }
    //std::sort(Pairs.begin(), Pairs.end(), less_than_cost());

    return Pairs;
}

void TextureEngine::SurfaceSimplificationEngine::changeVert(std::vector<Pair *> &Pairs, std::vector<Vertex *> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4 *> &listQ){
    /*
     * The other solution works but it takes too long (8 seconds for a model with 1.4m faces)
     *
     * */

    // 1 and 2
    unsigned int v_dash = Pairs[0]->vecPair.second;
    unsigned int v_remove = Pairs[0]->vecPair.first;

    // 3
    vs[v_dash] = &Pairs[0]->contVert;
    vs[v_remove] = vs[v_dash];
    // 4
    listQ[v_dash] = &Pairs[0]->Q;
    listQ[v_remove] = listQ[v_dash];
    // 5
    Pairs.erase(Pairs.begin());
}
































void TextureEngine::SurfaceSimplificationEngine::Optimize(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){
    unRepVert(vs, ind);
    std::vector<glm::mat4> ListQ = calcQMatrices2(vs, ind);
    std::vector<std::pair<unsigned int, unsigned int> > pairs = PairSelection2(ind);
    std::vector<Pair> Pairs = formPairList(vs,pairs, ListQ);
    // Note that number of pairs has to satisfy Faces + Vertices - Edges = 2 (if it is a Polyhedra)
    // Where faces = ind.size()/3 and vertices is just vs.size()

    int perc = vs.size()*.9;
    while(vs.size()>perc){
        changeVert(Pairs, vs, ind, ListQ);
        //Notice that after that, the number of Pairs should go down by at least 3 (if Vertex is not an edge)
    }
}

void TextureEngine::SurfaceSimplificationEngine::unRepVert(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

    /*
     * The idea of this algorithm is to implement this: https://stackoverflow.com/questions/2407451/find-unique-vertices-from-a-triangle-soup
     * The other two unRepVert are approximately O(n^2) (since there is a for loop inside a for loop)
     * This implementation however should accelerate things a bit and make it O(nlogn)
     * If we do std::unique(std::sort()) for the position of the vertices,
     * we will get a list of unique vertices
     * The next step then is to update the list of indeces
     *
     * This works way faster than the previous attempts and allegedely works on O(nlog(n))
     * */

    // Creates a vector of structs that join the vertex with their respective indeces
    std::vector<VertInd> vSort;
    for(unsigned int w = 0; w<ind.size(); w++){
        VertInd v;
        v.v = vs[w];
        v.ind = ind[w];
        vSort.push_back(v);
    }

    // This sorts the vertex with the indeces based on whether their position are the same
    std::sort(vSort.begin(), vSort.end(), VertSort());

    // Retrieve the list of vertex
    for(unsigned int w=0; w<vSort.size(); w++){
        vs[w] = vSort[w].v;
    }

    // Make sure that all of the vertices are unique
     vs.erase(std::unique(vs.begin(), vs.end(), vsComp()), vs.end());

     // Associates the new index i of each vertex to the right place in the ind array
     unsigned int count = 0;
     for(unsigned int w=0; w<vSort.size(); w++){
        ind[vSort[w].ind] = count;
        if(vSort[w].v.Pos == vSort[w+1].v.Pos){
            continue;
        }
        count++;
     }

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

std::vector<glm::mat4> TextureEngine::SurfaceSimplificationEngine::calcQMatrices2(std::vector<Vertex> &vs, std::vector<unsigned int> &ind){

    std::vector<std::vector<glm::mat4> > ListKp(vs.size());
    std::vector<glm::mat4> ListQ(vs.size());

    // Calculate the planes that define each face in the form vec4 (a,b,c,d) so that
    // ax + by + cz + d = 0, which defines a plane in (x,y,z)
    for(unsigned int k = 0; k<ind.size(); k++){
        unsigned int v1Ind = ind[k++];
        unsigned int v2Ind = ind[k++];
        unsigned int v3Ind = ind[k];
        // Calculate P for each plane (set of three vertices)
        glm::vec3 v1 = vs[v1Ind].Pos;
        glm::vec3 v2 = vs[v2Ind].Pos;
        glm::vec3 v3 = vs[v3Ind].Pos;
        // Notice that ax + by + cz + d = 0 for p = <a,b,c> is such that <a,b,c> is the vector normal to the plane
        glm::vec3 normal = glm::normalize(glm::cross(v1-v3,v2-v3)); // this is <a,b,c>
        float d = -glm::dot(v3, normal);
        glm::vec4 p = glm::vec4(normal,d);

        // Adding Kp to the three vertices
        ListKp[v1Ind].push_back(glm::outerProduct(p,p));
        ListKp[v2Ind].push_back(glm::outerProduct(p,p));
        ListKp[v3Ind].push_back(glm::outerProduct(p,p));
    }

    // Sum Kp of all vertices to find Q
    for(unsigned int k = 0; k<vs.size(); k++){
        bool first = true;
        for(unsigned i = 0; i<ListKp[k].size(); i++){
            if(first){
                ListQ[k] = ListKp[k][i];
                first = false;
            }
            else{
                glm::mat4 Q = ListKp[k][i];
                ListQ[k] = ListQ[k]+Q;
            }

        }

    }
    return ListQ;
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

void TextureEngine::SurfaceSimplificationEngine::changeVert2(std::vector<Pair> &Pairs, std::vector<Vertex> &vs, std::vector<unsigned int> &ind, std::vector<glm::mat4> &listQ){
    /*
     * The other solution works but it takes too long (8 seconds for a model with 1.4m faces)
     *
     * */

    // 1 and 2
    unsigned int v_dash = Pairs[0].vecPair.second;
    unsigned int v_remove = Pairs[0].vecPair.first;

    // 3
    vs[v_dash] = Pairs[0].contVert;
    vs[v_remove] = vs[v_dash];
    // 4
    listQ[v_dash] = Pairs[0].Q;
    listQ[v_remove] = listQ[v_dash];
    // 5
    Pairs.erase(Pairs.begin());
}

std::vector<std::pair<unsigned int, unsigned int> > TextureEngine::SurfaceSimplificationEngine::PairSelection(std::vector<unsigned int> &ind){
    std::vector<std::pair<unsigned int, unsigned int> > pairs;
    /*
     * The idea of this algorithm is to find all of the pairs of a mesh by comoparing all of them
     * This is is taking O(n^2), which is not good for a mesh with a lot of vertices
     * */
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

std::vector<std::pair<unsigned int, unsigned int> > TextureEngine::SurfaceSimplificationEngine::PairSelection2(std::vector<unsigned int> &ind){
    std::vector<std::pair<unsigned int, unsigned int> > pairs;
    /*
     * The idea of this algorithm is to find all of the pairs of a mesh
     * We first add all of the possible combinations
     * The result will be 2*n, where n is the number of edges (for a polyhedra)
     * Then we sort it in a way that duplicate edges will be in sequence
     * Then we use the same aporoach for unRepVert of unique(sort(array))
     * */
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
    unsigned k;
    for(k = 0; k<ind.size(); k++){
        v1 = ind[k++]; v2 = ind[k++]; v3=ind[k];



        // Makes sure that v1>v2>v3
        if(v3>v1){
            unsigned int temp = v1;
            v1 = v3;
            v3 = temp;
        }
        if(v3>v2){
            unsigned int temp = v2;
            v2 = v3;
            v3 = temp;
        }
        if(v2>v1){
            unsigned int temp = v1;
            v1 = v2;
            v2 = temp;
        }

        pairs.push_back(std::pair<unsigned int, unsigned int>(v1,v2));
        pairs.push_back(std::pair<unsigned int, unsigned int>(v1,v3));
        pairs.push_back(std::pair<unsigned int, unsigned int>(v2,v3));
    }

    // Now we sort Pairs according to their vlue
    // Keep in mind that pairs.first > pairs.second
    std::sort(pairs.begin(), pairs.end(), pairSort());
    pairs.erase(std::unique(pairs.begin(), pairs.end(), pairComp()), pairs.end());



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
    //std::sort(Pairs.begin(), Pairs.end(), less_than_cost());

    return Pairs;
}
