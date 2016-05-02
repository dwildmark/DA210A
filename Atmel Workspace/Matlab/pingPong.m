%----------------------------------------------|
% By Jonathan Böcker and David Tran            |
% 2016-03-20                                   |
%----------------------------------------------|
% Controller/plotting settings-----------------
P = '100';                                      % Values with a factor of 1000
I = '600';                                      % --||--
D = '200';                                      % --||--
SETDISTANCE = 300;                              % Setpoint in mm from sensor
OFFSET = '600';                                 % Offset 0-999
ANTIWINDUP = '100000';                          % Maximum integral sum
FREQUENCY = 20;                                 % Frequency for sampling(Hz)
COM_PORT = 'COM6';
BAUDRATE = 115200;
A = xlsread('bana4.xlsx');                      % Import conversion array
PLOT_TIME = 80;                                 % Window/plotting size in seconds
% Connect to Arduino---------------------------
a = serial(COM_PORT, 'BaudRate', BAUDRATE); 
set(a, 'Terminator', 10);
fopen(a);
% Initialize vectors and matrixes for plotting--
values = rand(FREQUENCY*PLOT_TIME,6);           % Matrix which will be plotted
tempValues = zeros(FREQUENCY,3);                % Matrix holding 1 sec of values
setpointvector = ones(FREQUENCY*PLOT_TIME,1).*SETDISTANCE;
plusFive = setpointvector.*1.05;
minusFive = setpointvector.*0.95;
values(:,4) = setpointvector;                   % Insert a static setpoint vector
values(:,5) = plusFive;                         % Vector 5 percent larger than SP
values(:,6) = minusFive;                        % Vector 5 percent smaller than SP
x = (1:FREQUENCY*PLOT_TIME)./FREQUENCY;         % x-vector for time vector
plotGraph = plot(x,values);                     % Set up Plot
pause(1)                                        % Let Matlab render
flushoutput(a);                                 % flush serial output/input 
flushinput(a);                                  % buffer before attempting to send
for index = 1:100                               % Transfer conversion array
    fprintf(a, num2str(A(index)));
end
fprintf(a, P);                                  % Transfer controller values
fprintf(a, I);                                  % --||--
fprintf(a, D);                                  % --||--
fprintf(a, num2str(SETDISTANCE));               % --||--
fprintf(a, OFFSET);                             % --||--
fprintf(a, ANTIWINDUP);                         % --||--
fprintf(a, num2str(1000/FREQUENCY));            % --||--
while ishandle(plotGraph)                       % While plotting window is active
    for index=1 : FREQUENCY                     % Read 1s samples 
        tempValues = circshift(tempValues, 1);  % Shift values 1 row downwards 
        for k=1 : 3                             % Read distance, then error, then output
            tempValues(1,k) = str2double(fscanf(a,'%c'));    
        end             
    end
    values = circshift(values, FREQUENCY);      % Shift big matrix to fit temporary matrix
    values(1:FREQUENCY,1:3) = tempValues;       % Insert temp matrix to plot latest values
    plotGraph = plot(x,values);                 % Plot latest values
    grid on
    legend('Distance [mm]','Error [mm]','Output (0-999)','Setpoint [mm]','Setpoint + 5%','Setpoint - 5%');
    xlabel('Seconds');
    ylabel('Values');
    title('Ball-controller');
    pause(0.5)                                  % Pause to let Matlab render
end
% Final plot after closing the continous plotting
fclose(a);
plot(x,values);
legend('Distance [mm]','Error [mm]','Output (0-999)','Setpoint [mm]','Setpoint + 5%','Setpoint - 5%');
grid on
xlabel('Seconds');
ylabel('Values');
title('Ball-controller');
