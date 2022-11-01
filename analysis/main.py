import subprocess

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

def createConfig(*, sz = (10, 10), players = 4, flags = None, red = None, blue = None):
  #TODO: Create config_parameters
  pass

if __name__ == '__main__':
  createConfig();
  print(run(Test['search']));
