#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <queue>
#include <climits>
#include <cstdlib>
using namespace std;

// 1단계 - 구조체 정의
struct Station {
    int id;
    string name;
    int line;
};

struct Edge {
    int to;
    double distance;
    int time;
    bool isTransfer;
};

// 2단계 - 전역 변수 선언
vector<Station> stations; //역 목록
vector<vector<Edge>> graph; // 각 역에서의 경로 목록
map<string, vector<int>> nameToIds; //이름으로 역 id 찾기(중복 호선)

int parseTime(string t) { // "2:00" -> 2분으로 변환
    int colon = t.find(':');
    int min = stoi(t.substr(0, colon));
    return min;
}

void loadCSV(string filename) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "파일 열기 실패" << endl;
        return;
    }
    string line;
    
    getline(file, line); // 첫 줄(헤더) 스킵

    int prevLine = -1; //자료에서 노선이 오름차순으로 나열되어있기 때문에 노선 변경되면 edge연결하면 안됨
    int prevId = -1;

    while (getline(file, line)) { // 한 줄씩 읽기 반복

        stringstream ss(line);
        string token;
        vector<string> tokens;

        
        while (getline(ss, token, ',')) { // ',' 대신 '\t'로 변경
            tokens.push_back(token);
        }
        
        Station s; //역 정보를 토큰으로부터 입력
        s.id = stations.size();
        s.name = tokens[2];
        s.line = stoi(tokens[1]);

        stations.push_back(s);
        graph.push_back(vector<Edge>()); 
        nameToIds[s.name].push_back(s.id);

        if(prevLine == s.line){ //아직 같은 호선을 추가중이면
            Edge e; 
            e.to = s.id;
            e.time = parseTime(tokens[3]);
            e.distance = stod(tokens[4]);
            e.isTransfer = false;
            graph[prevId].push_back(e); //직전 역과 지금 역의 경로를 연결

            Edge e2;
            e2.to = prevId;
            e2.time = parseTime(tokens[3]);
            e2.distance = stod(tokens[4]);
            e2.isTransfer = false;
            graph[s.id].push_back(e2);
        }
        prevId = s.id;
        prevLine = s.line;
    }
    file.close();
    cout << "총 역 수: " << stations.size() << endl; // 추가
}

void addTransferEdges() {
    for (auto& [name, ids] : nameToIds) { // nameToIds의 모든 항목 순회, name=역명 ids=해당 역명의 id목록
        if (ids.size() > 1) { // 같은 이름의 역이 2개 이상이면 환승역
            for (int i = 0; i < ids.size(); i++) { // 환승역의 모든 id 순회
                for (int j = 0; j < ids.size(); j++) { // 연결할 대상 id 순회
                    if (i != j) { // 자기 자신은 제외
                        Edge e;
                        e.to = ids[j];       // 연결할 대상 역 id
                        e.time = 5;          // 환승 소요시간 임의 설정 (5분)
                        e.distance = 0.5;      // 환승은 거리 0
                        e.isTransfer = true; // 환승 엣지 표시
                        graph[ids[i]].push_back(e); // ids[i] 역의 엣지 목록에 추가
                    }
                }
            }
        }
    }
}



vector<int> dijkstra(int start, int end) {
    vector<double> dist(stations.size(), INT_MAX); // 모든 역까지 거리 무한대로 초기화
    vector<int> prev(stations.size(), -1);          // 경로 추적용 (이전 역 저장)
    
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<>> pq; // 최소 힙
    
    dist[start] = 0;
    pq.push({0, start}); // {누적거리, 역id}
    int actualEnd = -1;

    while (!pq.empty()) {
        auto [cost, u] = pq.top(); // 가장 가까운 역 꺼내기
        pq.pop();
        
        if (stations[u].name == stations[end].name) {
            actualEnd = u; // 실제 도착한 역 id 저장하기
            break;
        }
        if (cost > dist[u]) continue; // 이미 더 짧은 경로 있으면 스킵
        
        for (Edge e : graph[u]) { // 인접 역 탐색
            double next = dist[u] + e.distance;
            if (next < dist[e.to]) { // 더 짧은 경로 발견하면
                dist[e.to] = next;
                prev[e.to] = u;      // 이전 역 저장
                pq.push({next, e.to});
            }
        }
    }
    
    // 경로 역추적
    vector<int> path;
    for (int cur = actualEnd; cur != -1; cur = prev[cur]) {
        path.push_back(cur);
    }

    reverse(path.begin(), path.end());
    return path;
}

int main(int argc, char* argv[]) {

    loadCSV("subway_utf8.csv");
    addTransferEdges();
    
    if (argc < 3) {
        cout << "사용법: ./metro 출발역 도착역" << endl;
        return 1;
    }

    string startName = argv[1];
    string endName = argv[2];

    // 역명으로 id 찾기
    if (nameToIds.find(startName) == nameToIds.end() ||
        nameToIds.find(endName) == nameToIds.end()) {
        cout << "존재하지 않는 역" << endl;
        return 0;
    }
    
    int startId = nameToIds[startName][0];
    int endId = nameToIds[endName][0];

    vector<int> path = dijkstra(startId, endId);
    
    // 경로 출력
    cout << "\n경로: ";
    for (int id : path) {
        cout << stations[id].name << "(" << stations[id].line << "호선)";
        if (id != path.back()) cout << " -> ";
    }
    cout << endl;

    ofstream out("path.txt");

    for (int id : path) {
        out << stations[id].name << "," << stations[id].line << "\n";
    }
    out.close();
    
    return 0;
}