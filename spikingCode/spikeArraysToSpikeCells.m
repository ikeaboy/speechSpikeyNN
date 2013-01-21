% swapTemp1 = HiddenSpikeTrain(13,1:T);
% swapTemp2 = HiddenSpikeTrain(32,1:T);
% HiddenSpikeTrain(13,1:T) = swapTemp2;
% HiddenSpikeTrain(32,1:T) = swapTemp1;
numOut=40;%100;%60; %number of output neurons.
SN.fnum=0;
outSN=cell(numOut,1);

for i=1:numOut
    outSN{i}=SN; % for output neuron states
end

outSNftime=cell(numOut,1);
% outSNftime{k}(outSN{k}.fnum) = t * Dt; %fire time record
fnum = 0;
for i = 1 : numOut
    for j = 1 : T
        if(TrainHiddenSpikeTrain(i,j) == 1)
            outSN{i}.fnum = outSN{i}.fnum + 1;
            outSNftime{i}(outSN{i}.fnum) = j;
        end
    end
end

figure
hold on
% axis([1 tmax 0 numIn+1])
% index = 0.001 : 0.001 : 81.8;
for i=1:numOut
    for j=1: size(outSNftime{i},2)  %dispplay output spikes
        plot([outSNftime{i}(j);outSNftime{i}(j)],[i;i+.5],'k');
%         plot(index,[outSNftime{i}(j);outSNftime{i}(j)],[i;i+.5],'k');
    end
end
xlabel('Hidden fire time')
ylabel('Hidden Neuron No.')