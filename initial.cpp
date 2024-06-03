#include<iostream>
#include<cstring>
#include<vector>
#include<queue>
#include<fstream>
using namespace std;
struct Edge
{
    int first;
    int second;
    char symbol;//epsilon = _
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
        else
        {
            cout << "Invalid command!" << endl;
        }
    }
}