import subprocess
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go

Test = {
  'game': 0,
  'search': 1,
};

Strat = {
  'seq': 0,
  'rr': 1,
  'shortest': 2,
  'ustedes': 3,
};

Team = {
  0: 'red', 'red': 0,
  1: 'blue', 'blue': 1,
}

def parseGame(parts):
  team, time = parts[0].split(' ')[2:];
  return Team[int(team)], int(time);

def parseSearch(parts):
  #TODO: Check valid result
  return [int(x) for x in parts[0].split(' ')[2:]]; #A: total/seq/thread

def run(test, strat = Strat['seq'], quantum = 1):
  out = subprocess.check_output(['build/tpso.an', str(test), str(strat), str(quantum)]);
  parts = out.decode('UTF-8').split('\n')[:-1];

  #TODO: Check for errors

  if test == Test['game']: return parseGame(parts);
  elif test == Test['search']: return parseSearch(parts);
  else: return None;

def createConfig(*, sz = (3, 3), players = 2, flags = None, teams = None):
  #TODO: Random if none
  flags = flags or ((0, 0), (2, 2));
  teams = teams or (((1, 0), (2, 0)), ((0, 2), (1, 2)));

  #TODO: Check values
  with open('./analysis/config_parameters.csv', mode = 'w') as f:
    f.write(f'{sz[0]} {sz[1]} {players}\n{flags[0][0]} {flags[0][1]}\n{flags[1][0]} {flags[1][1]}\n');
    f.writelines([f'{pos[0]} {pos[1]}\n' for team in teams for pos in team]);

def testSearch(r = 1):
  df = pd.DataFrame();
  fpath = f'./build/{r}_testSearch.csv.gz';
  try:
    df = pd.read_csv(fpath, index_col = 0);
  except:
    data = [];
    sz = [(10000, 10000)]; #TODO: Different sizes
    flags = [((0, 0), (0, 1)), ((7503, 0), (7503, 1)), ((9999, 9998), (9999, 9999))]; #TODO: More flag positions
    #TODO: More threads(players)
    for _sz in sz:
      for _flags in flags:
        createConfig(sz = _sz, flags = _flags);
        for _ in range(r):
          _data = run(Test['search']);
          data.append(list(_data) + [_sz, str(_flags[0])]);
        #TODO: Save each on it's own csv

    df = pd.DataFrame(data, columns = ['total', 'sequential', 'threaded', 'sz', 'flags']);
    df.to_csv(fpath);

  for col in ['total', 'sequential', 'threaded']: df[col] /= 1e6; #A: To ms #TODO: Not until printing
  for col in ['sequential', 'threaded']: df[f'{col}_pct'] = df[col] / df['total'];
  df['ratio'] = df['sequential'] / df['threaded'];
  # df.rename({'sequential': 'Secuencial', 'threaded': 'Threaded'}, axis = 1, inplace = True); #TODO

  return df;

if __name__ == '__main__':
  r = 1000;
  df = testSearch(r);
  df_close = df[df['flags'] == str((0, 0))];
  df_mid = df[df['flags'] == str((7503, 0))];
  df_far = df[df['flags'] == str((9999, 9998))];

  print(df_close.describe(), df_mid.describe(), df_far.describe(), sep = '\n');

  if False:
    fig = go.Figure();

    for _df in ['cerca']:#, 'medio', 'lejos']:
      for col in ['Secuencial', 'Threaded']:
        thisDf = None;
        if _df == 'cerca': thisDf = df_close;
        elif _df == 'medio': thisDf = df_mid;
        else: thisDf = df_far;
        fig.add_trace(go.Box(y = thisDf[col], name = f'{col} ({_df})'));

    fig.update_yaxes(type = 'log', title = 'Tiempo de búsqueda (ms, log)');
    fig.write_image('./build/img.png');
