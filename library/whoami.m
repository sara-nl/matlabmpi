function whoami()
% 
% --- whoami - version 1.0 ---
% 
% This function needs to be run at the start of each function/script to
% obtain the mpirank and mpisize as set by setmpistuff.m
%
% Copyright 2013 Jeroen Engelberts, SURFsara
%
mpisize = evalin('base','mpisize');
mpirank = evalin('base','mpirank');
assignin('caller','mpisize',mpisize);
assignin('caller','mpirank',mpirank);
