function getverbosity()
% 
% --- getverbosity - version 1.0 ---
% 
% This function retrieves the verbosity setting from the command line,
% specified via the optional '-v' flag.
%
% Copyright 2013 Jeroen Engelberts, SURFsara
%
verbosity = evalin('base','verbosity');
assignin('caller','verbosity',verbosity);
