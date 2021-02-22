#creaza simulatorul
set ns [new Simulator]
#deschide fisier rezultate
set nf [open out.nam w]
#instruieste ns sa logheze in fisierul de mai sus
$ns namtrace-all $nf

#creaza o procedura de terminare a NS
proc finish {} {
        global ns nf
        $ns flush-trace
    #Close the trace file
        close $nf
    #Execute nam on the trace file
        exec nam out.nam &
        exit 0
}

$ns at 5.0 "finish"

Agent/rtProto/DV set preference_        80

#seteaza protocolul de rutare sa fie distance-vector (daca am comenta aceasta linie se foloseste protocolul static, care este default)
$ns rtproto DV
#seteaza protocolul de rutare sa fie link-state
#$ns rtproto LS

Agent/rtProto/DV set preference_        80

#creaza noduri in retea
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#defineste legaturile de date intre noduri
$ns duplex-link $n0 $n1 100Kb 40ms DropTail
$ns duplex-link $n0 $n2 100Kb 100ms DropTail
$ns duplex-link $n0 $n3 100Kb 100ms DropTail
$ns duplex-link $n0 $n5 100Kb 100ms DropTail
$ns duplex-link $n1 $n2 100Kb 40ms DropTail
$ns duplex-link $n1 $n4 100Kb 100ms DropTail
$ns duplex-link $n2 $n5 100Kb 100ms DropTail
$ns duplex-link $n4 $n5 100Kb 100ms DropTail

#modifica costul legaturilor
$ns cost $n1 $n2 30 
$ns cost $n1 $n4 2 
$ns cost $n4 $n5 4

#creaza un transmitator care foloseste protocolul UDP
set udp0 [new Agent/UDP]
#ataseaza transmitatorul nodului n0
$ns attach-agent $n5 $udp0

#creaza o sursa de pachete de rata constanta (constant bit rate CBR) care emite un pachet de 500 octeti la fiecare 100ms.
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set interval_ 0.1
$cbr0 attach-agent $udp0

#creaza un receptor care pur si simplu "pierde" pachete
set null0 [new Agent/Null] 
#ataseaza-l nodului n2
$ns attach-agent $n3 $null0

#conecteaza transmitatorul si receptorul
$ns connect $udp0 $null0

#simuleaza un defect la secunda 2 pentru legatura n0 n3
$ns rtmodel-at 2.0 down $n0 $n3
#la secunda 3 legatura n0 n3 revine
$ns rtmodel-at 3.0 up $n0 $n3

#la secunda 0.5 incepem transferul de date
$ns at 1.0 "$cbr0 start"
#la secunda 4.5 oprim transferul de date
$ns at 5.0 "$cbr0 stop"

#incepe simularea
$ns run

