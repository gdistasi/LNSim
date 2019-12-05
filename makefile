
all: LNSimulator

GPP = g++

CPPOPTS = -std=c++11  -DDEBUG
#-pg --no-pie

OBJS = main.o PaymentRequest.o NetworkGenerator.o PaymentsGenerator.o PaymentChannel.o FeeCalculator.o PaymentChannelEndPoint.o Gateway.o LightningNetwork.o PaymentDeployer.o utils.o
OBJS += glpk/PaymentDeployerProportional.o glpk/PaymentDeployerMultipathExact.o SinglePathSolver.o MultipathHeuristic.o


ALL: LNSimulator

clean:
	rm -f *.o glpk/*.o LNSim

%.o: %.cpp
	$(GPP) $(CPPOPTS) -c $< -o $@ 

LNSimulator: $(OBJS)
	$(GPP) $(OBJS) -o LNSim $(CPPOPTS)
