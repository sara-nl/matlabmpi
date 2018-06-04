function matlabmain(verbosity,~)
%
% matlabmain - version 1.0
%
% Copyright 2013 Jeroen Engelberts, SURFsara
%
assignin('base','verbosity',verbosity);

% Obtain mpisize and mpirank
if ~(exist('mpisize','var')==1)
    whoami()
end

% On worker 0, create an input vector, necessary for all processes and 
% broadcast it to all others
if (mpirank == 0)
    disp('Hello from the master - I create a vector that I broadcast to all')
    a=[1,2,3,4,5,6,7,8,9]
    bcastvar(0,a);
else
    % All other processes (>0) receive the input vector from 0
    a=bcastvar(0);
end

if (mpirank == 0)
    % Distribute "the work": send each mpiworker a constant (mpirank*2) to
    % multiply with the distributed vector a
    for i = 1:mpisize-1
        sendvar(i,i*2);
    end
else
    c=receivevar(0);
end

if (mpirank == 0)
    % Create an empty result matrix to hold the result to be received from
    % the workers.
    result=zeros(mpisize-1,9);
    for i = 1:mpisize-1
        % Start receiving results - Note that the order may be different
        % depending on the speed with which the workers send back there results.
        result(i,:)=receivevar();
    end
    disp('Hello from the master - The final result I assembled is')
    result
else
    % Doing the local work here (here: a single multiplication)
    localresult=f1(a,c);
    disp(sprintf('Hello from process %d - I will multiply the input vector with %d\n',mpirank,c));
    % Send back the result to the "root" process 0
    sendvar(0,localresult);
end
end

