import Numeric as num

class bestprof:
    def __init__(self, filenm):
        infile = open(filenm)
        self.topo = 0
        self.profile = []
        for line in infile.readlines():
            if line[0]=="#":
                if line.startswith("# Input file"):
                    self.datnm = line.split("=")[-1][:-1]
                    continue
		if line.startswith("# Candidate"):
		    if line.startswith("# Candidate        =  PSR_"):
			self.psr = line.split("=")[-1].split("_")[1][:-1]
			continue
		    else:
			self.psr = None
                if line.startswith("# T_sample"):
                    self.dt = float(line.split("=")[-1])
                    continue
                if line.startswith("# Data Folded"):
                    self.N = float(line.split("=")[-1])
                    continue 
                if line.startswith("# Data Avg"):
                    self.data_avg = float(line.split("=")[-1])
                    continue 
                if line.startswith("# Data StdDev"):
                    self.data_std = float(line.split("=")[-1])
                    continue 
                if line.startswith("# Profile Avg"):
                    self.prof_avg = float(line.split("=")[-1])
                    continue 
                if line.startswith("# Profile StdDev"):
                    self.prof_std = float(line.split("=")[-1])
                    continue 
                if line.startswith("# Epoch_topo"):
                    try:
                        self.epochi = float(line.split("=")[-1].split(".")[0])
                        self.epochf = float("0."+line.split("=")[-1].split(".")[1])
                        self.topo = 1
                    except ValueError:
                        pass
                    continue
                if (not self.topo and line.startswith("# Epoch_bary")):
                    try:
                        self.epochi = float(line.split("=")[-1].split(".")[0])
                        self.epochf = float("0."+line.split("=")[-1].split(".")[1])
                    except ValueError:
                        pass
                if ((not self.topo and line.startswith("# P_bary")) or
                     (self.topo and line.startswith("# P_topo"))):
                    self.p0 = float(line.split("=")[-1].split("+")[0])/1000.0
                    continue
                if ((not self.topo and line.startswith("# P'_bary")) or
                     (self.topo and line.startswith("# P'_topo"))):
                    self.p1 = float(line.split("=")[-1].split("+")[0])
                    continue
                if ((not self.topo and line.startswith("# P''_bary")) or
                     (self.topo and line.startswith("# P''_topo"))):
                    self.p2 = float(line.split("=")[-1].split("+")[0])
                    continue
            else:
                self.profile.append(float(line.split()[-1]))
        infile.close()
        self.T = self.dt*self.N
        self.proflen = len(self.profile)
    def normalize(self):
        normprof = num.asarray(self.profile)
        normprof -= min(normprof)
        normprof /= max(normprof)
        return normprof

