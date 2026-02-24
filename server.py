from flask import Flask, request, jsonify, send_file
import subprocess
import os

app = Flask(__name__)

@app.route('/')
def index():
    return send_file('index.html')

@app.route('/search', methods=['POST'])
def search():
    data = request.json
    start = data['start']
    end = data['end']
    
    # C++ 실행
    result = subprocess.run(
    ['/Users/seunghyunhan/myDrive/code/Metro/metro', start, end],
    capture_output=True, text=True,
    cwd='/Users/seunghyunhan/myDrive/code/Metro'
    )
    
    # path.txt 읽기
    path = []
    with open('path.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            name, line_num = line.split(',')
            path.append({'name': name, 'line': line_num})

# 시작역에서만 불필요한 환승 제거
    if len(path) >= 2 and path[0]['name'] == path[1]['name']:
        path.pop(0)
    
    return jsonify(path)

if __name__ == '__main__':
    app.run(debug=True, port=3000)