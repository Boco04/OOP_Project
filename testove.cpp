#include<iostream>
#include<cstring>
#include<vector>
#include<queue>
#include<fstream>
#include<stack>
using namespace std;
struct Edge
{
    int first;
    int second;
    char symbol;//epsilon = _
};

class File_Container
{
    private:
    ofstream file;
    string saved_data, unsaved_data;
    string filename;
    bool is_open;
    bool is_saved;
    bool is_written;

    public:
    File_Container(): saved_data(""), unsaved_data(""), is_open(false), is_saved(false), is_written(false){};

    void Open(const string& directory)
    {
        filename = directory;
        if(is_open)
        {
            cout << "Another file is already opened." << endl;
        }
        else
        {
            file.open(filename,ios::out);
            is_open = true;
            cout << "Successfully opened " << filename << endl;
        }
    }

    void Write()
    {
        string newData, useless;//реално useless=%
        getline(cin, newData,'%');
        unsaved_data += newData;
        is_written = true;
        getline(cin, useless);
        //cin >> useless;
    }

    void Save()
    {
        if(is_written)
        {
            is_saved = true;
            saved_data += unsaved_data;
            unsaved_data = "";
            is_written = false;
            cout << "Successfully saved " << filename << endl;
        }
        else
        {
            cout << "You've not written anything new!" << endl;
        }
    }

    void SaveAs(const string& directory)
    {
        ofstream new_file;
        new_file.open(directory, ios::out);
        new_file << saved_data + unsaved_data;
        cout << "Successfully saved the new data in " << directory << endl;
    }

    void Close()
    {
        if(!is_open)
        {
            cout << "No file is opened!" << endl;
            return;
        }
        else
        {
            file << saved_data;
            file.close();
            is_open = false;
            is_written = false;
            is_saved = false;
            saved_data = "";
            unsaved_data = "";
            cout << "Successfully closed " << filename << endl;
        }
    }
};

class Machine
{
    private:
    string name;//id на автомата
    vector<vector<Edge>> matrix;//списък на съседите
    vector<Edge> edges;//ребра
    int vertices;//брой на състоянията на автомата
    vector<int> final;//финалните състояния на автомата

    void fillMatrix()
    {
        matrix.resize(vertices);
        for(int i = 0;i<vertices;i++)
        {
            matrix[i].resize(0);
        }
        for(int i = 0;i<edges.size();i++)
        {
            int u = edges[i].first;
            matrix[u].push_back(edges[i]);
        }
    }

    vector<bool> BFS(int v) const
    {
        vector<bool> used(vertices,0);
        queue<int> Queue;
        Queue.push(v);
        used[v] = 1;
        while(!Queue.empty())
        {
            v = Queue.front();
            Queue.pop();
            int N = matrix[v].size();
            for(int i =0;i<N;i++)
            {
                int nV = matrix[v][i].second;
                if(used[nV]) continue;
                used[nV]=1;
                Queue.push(nV);
            }
        }
        return used;
    }
    
    vector<bool> _closure(int v) const
    {
        vector<bool> closure(vertices,0);
        queue<int> Queue;
        Queue.push(v);
        closure[v] = 1;
        while(!Queue.empty())
        {
            v = Queue.front();
            Queue.pop();
            int N = matrix[v].size();
            for(int i =0;i<N;i++)
            {
                int nV = matrix[v][i].second;
                if(closure[nV]) continue;
                if(matrix[v][i].symbol=='_')
                {
                    closure[nV] = 1;
                    Queue.push(nV);
                }
            }
        }
        return closure;
    }

    public:
    
    Machine(){}

    Machine(string n, vector<Edge> e, int v, vector<int> f):name(n),edges(e),vertices(v),final(f){};

    

    int getVertices() const
    {
        return vertices;
    }

    void shift(const int& n)
    {
        for(Edge& e:edges)
        {
            e.first+=n;
            e.second+=n;
        }
        for(int& i:final)
        {
            i+=n;
        }
    }

    Machine& operator=(const Machine& other)
    {
        if(this!=&other)
        {
            name = other.name;
            edges = other.edges;
            vertices = other.vertices;
            final = other.final;
        }
        return *this;
    }

    Machine copy()
    {
        Machine m(name,edges,vertices,final);
        return m;
    }

    vector<Edge>& getEdges()
    {
        return edges;
    }

    string getName() const
    {
        return name;
    }

    vector<int>& getFinal()
    {
        return final;
    }

    void SaveToFile(ostream& out)
    {
        out << name << endl;

        out << vertices << endl;

        out << final.size();

        for(int i:final)
        {
            out << " " << i;
        }

        out << endl;

        out << edges.size() << endl;

        for(Edge e:edges)
        {
            out << e.first << " " << e.second << " " << e.symbol << endl;
        }
    }

    void readData(istream& in)
    {
        in >> name;

        in >> vertices;

        int final_size;

        in >> final_size;

        final.resize(final_size);

        for(int i = 0;i<final.size();i++)
        {
            in >> final[i];
        }

        int edges_size;

        in >> edges_size;

        edges.resize(edges_size);

        for(int i = 0;i<edges.size();i++)
        {
            in >> edges[i].first >> edges[i].second >> edges[i].symbol;
        }
    }

    void printEdges() const
    {
        for(Edge e:edges)
        {
            cout << e.first<<"---"<<e.symbol << "---"  << e.second << endl;
        }
        cout << endl;
    }

    void changeStart(const int& start)
    {
        for(Edge& e:edges)
        {
            if(e.first == start)
            {
                e.first = 0;
            }
            else if(e.first<start)
            {
                e.first++;
            }
            if(e.second == start)
            {
                e.second = 0;
            }
            else if(e.second<start)
            {
                e.second++;
            }
        }
        for(int& i:final)
        {
            if(i==start)
            {
                i = 0;
            }
            else if(i<start)
            {
                i++;
            }
        }
    }

    bool isEmpty() //Проверка чрез BFS
    {
        fillMatrix();
        vector<bool> used = BFS(0);
        for(int i=0;i<final.size();i++)
        {
            if(used[final[i]]) return false;
        }
        return true;
    }

    bool isFinite()
    {
        fillMatrix();
        vector<bool> _reachable = BFS(0);
        for(int i =0;i<edges.size();i++)
        {
            int u = edges[i].first;
            int v = edges[i].second;
            if(_reachable[u] && edges[i].symbol!='_')
            {
                vector<bool> reachable_v = BFS(v);
                if(!reachable_v[u]) continue;
                for(int j:final)
                {
                    if(reachable_v[j]) return false;
                }
            }
        }
        return true;
    }

    bool isDeterministic()
    {
        bool digits[10];
        bool letters[26];
        fillMatrix();
        for(int i = 0;i<vertices;i++)
        {
            for(int d = 0;d<10;d++)
            {
                digits[d] = false;
            }
            for(int l = 0;l<26;l++)
            {
                letters[l] = false;
            }
            for(Edge edge:matrix[i])
            {
                if(edge.symbol=='_') return false;
                else if (edge.symbol >= '0' && edge.symbol <= '9')
                {
                    if(digits[edge.symbol-'0']) return false;
                    else digits[edge.symbol-'0']= true;
                }
                else
                {
                    if(letters[edge.symbol-'a']) return false;
                    else letters[edge.symbol-'a']= true;
                }
            }
        }
        return true;
    }

    bool recognize(const string& word)
    {
        fillMatrix();
        vector<vector<bool>> used;
        used.resize(vertices);
        for(int i =0;i<vertices;i++)
        {
            used[i].resize(word.size()+1,0);
        }
        queue<pair<int,int>> Queue;
        Queue.push({0,0});
        used[0][0] = true;
        while(!Queue.empty())
        {
            int v = Queue.front().first;
            int pos = Queue.front().second;
            Queue.pop();
            for(Edge edges:matrix[v])
            {
                int nV = edges.second;
                char nS = edges.symbol;
                if(nS=='_')
                {
                    if(used[nV][pos]) continue;
                    used[nV][pos]=true;
                    Queue.push({nV,pos});
                }
                else
                {
                    if(pos==word.size()) continue;
                    if(word[pos]!=nS) continue;
                    if(used[nV][pos+1]) continue;
                    Queue.push({nV,pos+1});
                    used[nV][pos+1]=true;
                }
            }
        }
        for(int f: final)
        {
            if(used[f][word.size()]) return true;
        }
        return false;
    }

    string un(vector<Machine>& v)
    {
        fillMatrix();
        Machine ans = this->copy();
        for(int i:final)
        {
            ans.edges.push_back({i,0,'_'});
        }
        v.push_back(ans);
        return "("+ this->name + "+)";
    }

    Machine star()//звезда на Клини 
        {
            fillMatrix();
            Machine ans = this->copy();
            for(int i:final)
            {
                ans.edges.push_back({i,0,'_'});
            }
            ans.vertices++;
            ans.final.push_back(ans.vertices-1);
            ans.edges.push_back({0,ans.vertices-1,'_'});
            ans.name = name + "*";
            return ans;
        }

    Machine& FirstT()//премахване на епсилон преходите
    {
        fillMatrix();
        vector<Edge> newEdges;
        vector<int> f;
        for(int i = 0;i<vertices;i++)
        {
            bool flag = false;
            vector<bool> i_closure = _closure(i);
            for(int j =0;j<vertices;j++)
            {
                for(int k = 0;k<final.size();k++)
                { 
                    if(i_closure[j]&&j==final[k])
                    {
                        f.push_back(i);
                        flag = true; 
                        break;
                    }
                }
                if(flag) break;
            }
            for(char c = 'a'; c = 'z'; c++)
            {
                vector<bool> is_reachable_with_c_from_i(vertices,0);
                for(int j = 0; j < vertices;j++) 
                {
                    for(int k = 0;k<matrix[j].size();k++)
                    {
                        if(i_closure[j]&&matrix[j][k].symbol==c)
                        {
                            if(is_reachable_with_c_from_i[matrix[j][k].second]) continue;
                            is_reachable_with_c_from_i[matrix[j][k].second] = 1;
                        }
                    }
                }
                vector<bool> ext_closure(vertices,0);
                for(int j = 0;j<vertices;j++)
                {
                    if(!is_reachable_with_c_from_i[j]) continue;
                    vector<bool> closure_j = _closure(j);
                    for(int k = 0;k< vertices;k++)
                    {
                        if(closure_j[k])
                        {
                            ext_closure[k] = 1;
                        }
                    }
                }
                for(int j = 0;j<vertices;j++)
                {
                    if(!ext_closure[j]) continue;
                    newEdges.push_back({i,j,c});
                }
            }
            for(char c = '0'; c = '9'; c++)
            {
                vector<bool> is_reachable_with_c_from_i(vertices,0);
                for(int j = 0; j < vertices;j++) 
                {
                    for(int k = 0;k<matrix[j].size();k++)
                    {
                        if(i_closure[j]&&matrix[j][k].symbol==c)
                        {
                            if(is_reachable_with_c_from_i[matrix[j][k].second]) continue;
                            is_reachable_with_c_from_i[matrix[j][k].second] = 1;
                        }
                    }
                }
                vector<bool> ext_closure(vertices,0);
                for(int j = 0;j<vertices;j++)
                {
                    if(!is_reachable_with_c_from_i[j]) continue;
                    vector<bool> closure_j = _closure(j);
                    for(int k = 0;k< vertices;k++)
                    {
                        if(closure_j[k])
                        {
                            ext_closure[k] = 1;
                        }
                    }
                }
                for(int j = 0;j<vertices;j++)
                {
                    if(!ext_closure[j]) continue;
                    newEdges.push_back({i,j,c});
                }
            }
        }
        edges = newEdges;
        final = f;
        return *this;
    }

    Machine& SecondT()//детерминиране чрез Рабин-Скот
    {
        name = "Determined->" + name;
        vector<int> a(vertices, 0);//powers of two
        a[0] = 1;
        for(int i =1;i<vertices;i++)
        {
            a[i] = 2*a[i-1];
        }
        vector<Edge> e;
        int v = 2*a[vertices-1];
        vector<int> f;
        vector<bool> isFinal(v,0);
        for(int i = 0;i<2*a[vertices-1];i++)
        {
            vector<bool> bits(vertices,0);//bynary representation of i
            for(int j =0;j<vertices;j++)
            {
                if(i%(2*a[j])>=a[j])
                {
                    bits[j] = 1;
                }
            }
            for(int j = 0 ; j<vertices;j++)
            {
                for(int k = 0; k<final.size();k++)
                {
                    if(bits[j]==1&&j==final[k]) isFinal[i] = 1;
                }
            }
            for(char c = 'a'; c <= 'b';c++)//за тестове е по-удобно азбуката да е {a,b}, иначе става огромно
            {
                int OtherVertex = 0;
                vector<bool> OtherVertexBits(vertices,0);
                for(Edge edge:edges)
                {
                    if(bits[edge.first]&&edge.symbol==c)
                    {
                        OtherVertexBits[edge.second] = 1;
                    }
                }
                for(int i =0;i<vertices;i++)
                {
                    if(OtherVertexBits[i])
                    OtherVertex+=a[i];
                }
                e.push_back({i,OtherVertex,c});
            }
            /*for(char c = '0'; c <= '9';c++)
            {
                int OtherVertex = 0;
                vector<bool> OtherVertexBits(vertices,0);
                for(Edge edge:edges)
                {
                    if(bits[edge.first]&&edge.symbol==c)
                    {
                        OtherVertexBits[edge.second] = 1;
                    }
                }
                for(int i =0;i<vertices;i++)
                {
                    if(OtherVertexBits[i])
                    OtherVertex+=a[i];
                }
                e.push_back({i,OtherVertex,c});
                e.push_back({0,0,c});
            }//Дотук сме направили ребрата, записали се сме кои са финалните състояния, но началото е номер 1 - ще го сменим да е 0*/
        }
        for(int i =0;i<v;i++)
        {
            if(isFinal[i]) f.push_back(i);//Вече вкарахме и финалните!
        }
        edges = e;
        final = f;
        vertices = v;
        changeStart(1);
        return *this;
    }

    Machine& transform()//мутатор
    {
        FirstT();
        SecondT();
        return *this;
    }
};

string Union(Machine& m1, Machine& m2, vector<Machine>& m)
{
    Machine m3 = m1.copy();
    m3.shift(m2.getVertices()+1);
    Machine m4 = m2.copy();
    m4.shift(1);
    int v = m3.getVertices() + m4.getVertices()+1;
    vector<Edge> e;
    for(Edge E:m3.getEdges())
    {
        e.push_back(E);
    }
    for(Edge E:m4.getEdges())
    {
        e.push_back(E);
    }
    e.push_back({0,1,'_'});
    e.push_back({0,m4.getVertices()+1,'_'});
    vector<int> f;
    for(int F:m3.getFinal())
    {
        f.push_back(F);
    }
    for(int F:m4.getFinal())
    {
        f.push_back(F);
    }
    string n = "(" + m3.getName() + "_U_" + m4.getName() + ")";
    m.push_back(Machine(n,e,v,f));
    return n;
}

string Concat(Machine& m1, Machine& m2, vector<Machine>& m)
{
    Machine m4 = m2.copy();
    m4.shift(m1.getVertices());
    int v = m1.getVertices() + m4.getVertices();
    vector<Edge> e;
    for(Edge E:m1.getEdges())
    {
        e.push_back(E);
    }
    for(Edge E:m4.getEdges())
    {
        e.push_back(E);
    }
    vector<int> f;
    for(int F:m1.getFinal())
    {
        e.push_back({F,m1.getVertices(),'_'});
    }
    for(int F:m4.getFinal())
    {
        f.push_back(F);
    }
    string n = "(" + m1.getName() + "_._" + m4.getName() + ")";
    m.push_back(Machine(n,e,v,f));
    return n;
}

Machine& findMachine(string id,vector<Machine>& machines)
{
    for(int i=0;i<machines.size();i++)
    {
        if(machines[i].getName()==id)
        {
            return machines[i];
        }
    }
    Machine m;
    machines.push_back(m);
    return machines[machines.size()-1];
}

Machine create_by_word(string t)
{
    if(t=="/") return Machine("", {}, 1, {});
    Machine res("", {}, t.size()+1, {(int)t.size()});
    for(int j=0; j<t.size(); j++)
    {
        res.getEdges().push_back({j, j+1, t[j]});
    }
    return res;
}

Machine evaluate(string t)
{
    stack<int> operators;
    vector<Machine> operands;

        /// * = 1
        /// . = 2
        /// | = 3

    for(int i=0; i<t.size(); i++)
    {
        if(t[i]=='(')
        {
            int j=i, count=1;
            while(true)
            {
                j++;
                if(t[j]=='(') count++;
                if(t[j]==')') count--;
                if(count==0) break;
            }
            operands.push_back(evaluate(t.substr(i+1, j-i-1)));//((s).((af)*)).(bcd)
            i=j;
            continue;
        }
        if(('a'<=t[i] && t[i]<='z') || ('0'<=t[i] && t[i]<='9') || t[i]=='_' || t[i]=='/') return create_by_word(t);
        int oprt;
        if(t[i]=='|') oprt=3;
        if(t[i]=='.') oprt=2;
        if(t[i]=='*') oprt=1;
        while(!operators.empty() && operators.top()<=oprt)
        {
            int last=operators.top();
            operators.pop();
            if(last==1) operands[operands.size()-1]=operands[operands.size()-1].star();
            if(last==2)
            {
                Machine sec=operands[operands.size()-1];
                Machine fir=operands[operands.size()-2];
                operands.resize(operands.size()-2);
                Concat(fir, sec, operands);
            }
            if(last==3)
            {
                Machine sec=operands[operands.size()-1];
                Machine fir=operands[operands.size()-2];
                operands.resize(operands.size()-2);
                Union(fir, sec, operands);
            }
        }
        operators.push(oprt);
    }
    while(!operators.empty())
    {
        int last=operators.top();
        operators.pop();
        if(last==1) operands[operands.size()-1]=operands[operands.size()-1].star();
        if(last==2)
        {
            Machine sec=operands[operands.size()-1];
            Machine fir=operands[operands.size()-2];
            operands.resize(operands.size()-2);
            Concat(fir, sec, operands);
        }
        if(last==3)
        {
            Machine sec=operands[operands.size()-1];
            Machine fir=operands[operands.size()-2];
            operands.resize(operands.size()-2);
            Union(fir, sec, operands);
        }
    }
    Machine res("Machine of <" + t + ">", operands[0].getEdges(), operands[0].getVertices(), operands[0].getFinal());
    return res;
}

void savetoFile(string fileName, string id, vector<Machine>& machines)
{
    for(Machine m:machines)
    {
        if(id==m.getName())
        {
            ofstream FILE(fileName+".Klopp");
            m.SaveToFile(FILE);
        }
    }
}

int main()
{
    create_by_word("abbba").printEdges();
    cout << "----";
    evaluate("(ab)*|(aa)").printEdges();
    Machine m = create_by_word("abbba");
    m.printEdges();
    vector<Machine> machines;
    string entry;
    while(true)
    {
        getline(cin,entry);
        if(entry=="list")
        {
            for(Machine m:machines)
            {
                cout << m.getName() << ",";
            }
            cout << endl;
        }

        else if(entry.size()>=5 && entry.substr(0,5)=="print")
        {
            entry.erase(0,6);
            findMachine(entry,machines).printEdges();
        }

        else if(entry.size()>=4 && entry.substr(0,4)=="save")
        {
            entry.erase(0,5);
            int pos = 0;
            for(;pos<entry.size();pos++)
            {
                if(entry[pos]==' ') break;
            }
            string id = entry.substr(0,pos);
            entry.erase(0,pos+1);
            string fileName = entry;
            savetoFile(fileName,id,machines);
        }

        else if(entry.size()>=4 && entry.substr(0,4)=="read")
        {
            entry.erase(0,5);
            string fileName = entry;
            ifstream infile(entry);
            findMachine(fileName,machines).readData(infile);
        }

        else if(entry=="exit")
        {
            cout << "Exiting the program..." << endl;
            return 0;
        }

        else if(entry.size()>= 5 && entry.substr(0,5)=="empty")
        {
            entry.erase(0,6);
            cout << findMachine(entry,machines).isEmpty() << endl;
        }

        else if(entry.size()>=6 && entry.substr(0,6)=="finite")
        {
            entry.erase(0,7);
            cout << findMachine(entry,machines).isFinite() << endl;
        }

        else if(entry.size()>= 13 && entry.substr(0,13) == "deterministic")
        {
            entry.erase(0,14);
            cout << findMachine(entry,machines).isDeterministic() << endl;            
        }

        else if(entry.size()>=9 && entry.substr(0,9)=="recognize")
        {
            entry.erase(0,10);
            int pos = 0;
            for(;pos<entry.size();pos++)
            {
                if(entry[pos]==' ') break;
            }
            string id = entry.substr(0,pos);
            entry.erase(0,pos+1);
            string word = entry;
            cout << findMachine(id,machines).recognize(word) << endl;
        }

        else if(entry.size()>=5 && entry.substr(0,5)=="union")
        {
            entry.erase(0,6);
            int pos = 0;
            while(entry[pos]!=' ')
            {
                pos++;
            }
            string id1 = entry.substr(0,pos);
            entry.erase(0,pos+1);
            string id2 = entry;
            cout << Union(findMachine(id1,machines),findMachine(id2,machines),machines) << endl;
        }

        else if(entry.size()>=6 && entry.substr(0,6)=="concat")
        {
            entry.erase(0,7);
            int pos = 0;
            while(entry[pos]!=' ')
            {
                pos++;
            }
            string id1 = entry.substr(0,pos);
            entry.erase(0,pos+1);
            string id2 = entry;
            cout << Concat(findMachine(id1,machines),findMachine(id2,machines),machines) << endl;
        }

        else if(entry=="proverka")
        {
            findMachine("Az",machines).FirstT().getEdges();
        }
        else

        {
            cout << "Invalid command!" << endl;
        }
    }
}