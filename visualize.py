import os
import webbrowser

# path.txt 읽기
path = []
with open('path.txt', 'r') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        name, line_num = line.split(',')
        path.append((name, line_num))

# 중복 역명 제거
unique_path = []
seen_names = set()
for p in path:
    if p[0] not in seen_names:
        unique_path.append(p)
        seen_names.add(p[0])

# 호선별 색상 (그라데이션용)
line_colors = {
    '1': '#2563eb',
    '2': '#16a34a',
    '3': '#ea580c',
    '4': '#0891b2',
    '5': '#7c3aed',
    '6': '#92400e',
    '7': '#4d7c0f',
    '8': '#be185d',
}

# 역 카드 HTML 생성
cards_html = ''
for i, (name, line_num) in enumerate(unique_path):
    color = line_colors.get(line_num, '#6b7280')
    is_start = i == 0
    is_end = i == len(unique_path) - 1

    badge = ''
    if is_start:
        badge = '<span class="badge start">출발</span>'
    elif is_end:
        badge = '<span class="badge end">도착</span>'
    else:
        badge = f'<span class="badge line" style="background:{color}22;color:{color};border-color:{color}44">{line_num}호선</span>'

    arrow = '<div class="arrow">↓</div>' if i < len(unique_path) - 1 else ''

    cards_html += f'''
    <div class="station-card {'start-card' if is_start else 'end-card' if is_end else ''}" style="--accent:{color}">
        <div class="station-dot" style="background:{color}"></div>
        <div class="station-info">
            <div class="station-name">{name}</div>
            {badge}
        </div>
    </div>
    {arrow}
    '''

total = len(unique_path)
transfers = len(set(p[1] for p in unique_path)) - 1

html = f'''<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="UTF-8">
<title>지하철 경로</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Noto+Sans+KR:wght@300;400;500;700&display=swap');

  * {{ margin: 0; padding: 0; box-sizing: border-box; }}

  body {{
    min-height: 100vh;
    background: linear-gradient(135deg, #1a1a2e 0%, #16213e 40%, #0f3460 100%);
    font-family: 'Noto Sans KR', sans-serif;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 40px 20px;
  }}

  .container {{
    width: 100%;
    max-width: 480px;
  }}

  .header {{
    text-align: center;
    margin-bottom: 32px;
  }}

  .header h1 {{
    font-size: 22px;
    font-weight: 700;
    color: white;
    margin-bottom: 8px;
    letter-spacing: -0.02em;
  }}

  .header .meta {{
    font-size: 13px;
    color: rgba(255,255,255,0.45);
    letter-spacing: 0.05em;
  }}

  .glass-card {{
    background: rgba(255,255,255,0.06);
    backdrop-filter: blur(20px);
    -webkit-backdrop-filter: blur(20px);
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 24px;
    padding: 28px;
    box-shadow: 0 8px 32px rgba(0,0,0,0.3);
  }}

  .stats {{
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
    margin-bottom: 28px;
  }}

  .stat {{
    background: rgba(255,255,255,0.05);
    border: 1px solid rgba(255,255,255,0.08);
    border-radius: 16px;
    padding: 16px;
    text-align: center;
  }}

  .stat-value {{
    font-size: 28px;
    font-weight: 700;
    color: white;
    line-height: 1;
    margin-bottom: 4px;
  }}

  .stat-label {{
    font-size: 11px;
    color: rgba(255,255,255,0.4);
    letter-spacing: 0.08em;
  }}

  .path-container {{
    display: flex;
    flex-direction: column;
    align-items: stretch;
  }}

  .station-card {{
    display: flex;
    align-items: center;
    gap: 14px;
    padding: 14px 16px;
    background: rgba(255,255,255,0.04);
    border: 1px solid rgba(255,255,255,0.07);
    border-radius: 14px;
    transition: all 0.2s;
    border-left: 3px solid var(--accent);
  }}

  .start-card {{
    background: rgba(255,255,255,0.09);
    border-color: rgba(255,255,255,0.15);
  }}

  .end-card {{
    background: rgba(255,255,255,0.09);
    border-color: rgba(255,255,255,0.15);
  }}

  .station-dot {{
    width: 10px;
    height: 10px;
    border-radius: 50%;
    flex-shrink: 0;
    box-shadow: 0 0 8px currentColor;
  }}

  .station-info {{
    display: flex;
    align-items: center;
    justify-content: space-between;
    width: 100%;
  }}

  .station-name {{
    font-size: 15px;
    font-weight: 500;
    color: white;
    letter-spacing: -0.01em;
  }}

  .badge {{
    font-size: 10px;
    padding: 3px 8px;
    border-radius: 20px;
    font-weight: 500;
    letter-spacing: 0.05em;
    border: 1px solid;
  }}

  .badge.start {{
    background: rgba(16,185,129,0.15);
    color: #10b981;
    border-color: rgba(16,185,129,0.3);
  }}

  .badge.end {{
    background: rgba(239,68,68,0.15);
    color: #ef4444;
    border-color: rgba(239,68,68,0.3);
  }}

  .badge.line {{
    font-weight: 600;
  }}

  .arrow {{
    text-align: center;
    color: rgba(255,255,255,0.2);
    font-size: 14px;
    padding: 4px 0;
    line-height: 1;
  }}
</style>
</head>
<body>
<div class="container">
  <div class="header">
    <h1>🚇 {unique_path[0][0]} → {unique_path[-1][0]}</h1>
    <div class="meta">최단거리 경로</div>
  </div>
  <div class="glass-card">
    <div class="stats">
      <div class="stat">
        <div class="stat-value">{total}</div>
        <div class="stat-label">총 역 수</div>
      </div>
      <div class="stat">
        <div class="stat-value">{transfers}</div>
        <div class="stat-label">환승 횟수</div>
      </div>
    </div>
    <div class="path-container">
      {cards_html}
    </div>
  </div>
</div>
</body>
</html>'''

# HTML 파일 저장
with open('path_result.html', 'w', encoding='utf-8') as f:
    f.write(html)

# 브라우저로 열기
webbrowser.open('file://' + os.path.abspath('path_result.html'))