function setmpistuff(mpisize,mpirank)
% 
% --- setmpistuff - version 1.0 ---
% 
% This function is used by matlabmpi.c to set the variables mpirank and
% mpisize in the workspace base.
%
% Copyright 2013 Jeroen Engelberts, SURFsara
%
assignin('base','mpisize',mpisize);
assignin('base','mpirank',mpirank);
