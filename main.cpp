#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<climits>
#include<queue>
#include<fstream>
#include<sstream>
using namespace std;

struct Station{ //역
    int id;
    string name;
    int line;
};

struct Edge{ //경로
    int to;
    double distance;
    bool isTransfer;
};

vector<Station> stations; //전체 역을 담을 벡터 생성
vector<vector<Edge>> graph; // 각 역에서의 경로 목록 ex) graph[0]은 stations[0]에서 갈 수 있는 경로 목록
map<string, vector<int>> nameToIds; //string을 기준으로 해당 역의 id들 찾는 map. 환승역일 경우 호선마다 역을 다르게 여기기에 필요

void loadCSV(string filename) //csv 파일을 불러와서 읽는 함수
{
    ifstream file(filename);

    if(!file.is_open())
    {
        cout<<"파일 열기 실패"<<endl;
        return;
    }

    string line;
    getline(file, line); //csv 파일의 첫 줄(헤더) 제외

    int prevLine = -1; //순차적으로 1호선부터 8호선까지 읽기 때문에 노선 변경시 엣지 연결하면 안됨.
    int prevId = -1; // 그리고 양방향으로 연결해야 하기 때문에 직전 역의 정보를 알아야 함.

    while(getline(file, line))
    {
        stringstream ss(line); //문자열을 스트림처럼 읽기 위해서 ss에 넣기
        string token;
        vector<string> tokens;

        while(getline(ss, token, ','))
        {
            tokens.push_back(token);    
        }

        Station s; //역 정보를 담을 임시 Station 생성
        s.id = stations.size(); //역 id == 크기 
        s.name = tokens[2]; //역 이름
        s.line = stoi(tokens[1]); //stoi를 통해 string을 int로 변환

        stations.push_back(s); //역 s를 삽입
        graph.push_back(vector<Edge>()); //빈 엣지 벡터 만들어두기. 크기 맞추기 위함
        nameToIds[s.name].push_back(s.id); //해당 역 이름의 위치에 아이디 삽입

        if(prevLine == s.line) //이전 역과 현재 역의 호선이 같으면
        {
            Edge e;
            e.to = s.id;
            e.distance = stod(tokens[4]); //stod을 통해 string을 double로 변환
            e.isTransfer = false;
            graph[prevId].push_back(e); //graph의 직전 역 자리에 해당 엣지를 추가함으로서 {전역 -> 현재역} 엣지 추가
            
            Edge e2;
            e2.to = prevId;
            e2.distance = stod(tokens[4]);
            e2.isTransfer = false;
            graph[s.id].push_back(e2); //graph의 현재 역 자리에 해당 엣지를 추가함으로서 {현재역 -> 전역} 엣지 추가
        }
        prevLine = s.line;
        prevId = s.id;
    }
    file.close();
}

void addTransferEdges() //동일한 역이나 호선이 달라 다른 역으로 여겨지는 환승역 연결하는 함수
{
    for(auto& [name, ids]: nameToIds) //nameToIds의 모든 항목들 순회
    {
        if(ids.size()>1) //같은 이름의 역이 2개 이상이면, which means 환승역이면
        {
            for(int i=0;i<ids.size();i++) //i가 연결 시작점
            {
                for(int j=0;j<ids.size();j++) //j가 연결 대상
                {
                    if(i!=j)
                    {
                        Edge e;
                        e.to = ids[j]; //연결 대상 역 id
                        e.distance = 2.5; //환승 거리는 임의로 2.5 설정해서 약간의 패널티. 너무 낮췄더니 환승을 너무 자주 함.
                        e.isTransfer = true; //환승 true
                        graph[ids[i]].push_back(e); //이중 for문이라 양방향 모두 생성됨
                    }
                }
            }
        }
    }
}

vector<int> dijkstra(int start, int end) //최단거리 찾기 알고리즘
{
    vector<double> dist(stations.size(), INT_MAX); //역 개수만큼 크기, 거리 무한대로 초기화
    vector<int> prev(stations.size(), -1); //경로 추적(이전 역 저장)용 vector(역 개수만큼 크기, 모든 값 -1로 초기화)

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq; //우선순위 큐 생성(작은 값 먼저 나오도록 greater<>, 누적거리와 역 id 쌍)

    dist[start] = 0; //출발역은 거리 0으로 초기화
    pq.push({0, start}); //누적거리 0, 시작역 큐에 push

    int actualEnd = -1; //환승역이 도착역일 때 실제 도달 호선의 id 저장

    while(!pq.empty()) //빌 때까지
    {
        auto[cost, u] = pq.top(); //cost == 누적거리, u == 역 id
        pq.pop();

        if(stations[u].name == stations[end].name) //도착역 도착하면
        {
            actualEnd = u; //실제 도착한 역 id 저장하기(환승역일 시 다른 호선과 바뀔 위험)
            break;
        }

        if(cost>dist[u]) //이미 해당 역에 더 짧은 경로가 존재할 시 스킵
        {
            continue;
        }

        for(Edge e : graph[u]) //u 역의 엣지 다 탐색
        {
            double next = dist[u] + e.distance;
            if(next < dist[e.to]) //만약 e로 향하는 더 짧은 경로 발견 시
            {
                dist[e.to]=next;
                prev[e.to]=u; //이전 역을 저장
                pq.push({next, e.to});
            }
        }
    }

    vector<int> path; //역추적 위한 vector
    for(int cur = actualEnd; cur != -1; cur = prev[cur]) //역추적해서 push
    {
        path.push_back(cur);
    }
    reverse(path.begin(), path.end()); //거꾸로
    return path;
}

int main(int argc, char* argv[])
{
    loadCSV("subway_utf8.csv");
    addTransferEdges();

    if(argc < 3) //넘어온 인자 적으면
    {
        cout<<"사용법: ./metro 출발역 도착역" << endl;
        return 1;
    }

    string startName = argv[1]; //출발, 도착 역 이름
    string endName = argv[2];

    if(nameToIds.find(startName) == nameToIds.end() || 
       nameToIds.find(endName) == nameToIds.end())
    {
            cout << "존재하지 않는 역" << endl;
            return 0;
    }

    int startId = nameToIds[startName][0]; //출발, 도착 id
    int endId = nameToIds[endName][0];

    vector<int> path = dijkstra(startId, endId);

    ofstream out("path.txt");

    for(int id:path) //path.txt 파일에 입력
    {
        out << stations[id].name << "," << stations[id].line << "\n";
    }

    out.close();
    return 0;
}
