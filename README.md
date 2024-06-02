# OOP_Project
Формат за въвеждане на автомат във файл:
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
