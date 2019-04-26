# deformationTransfer

Testing:

Im Ordner Testing befinden sich skripte, die zeigen, wie die Argumente an die einzelnen Tools übergeben werden sollen. 


Grundsätzlich ist der Ablauf wie folgt:

mit dem CorresTool werden manuell korrespondierende Vertices annotiert. 
Dazu navigiert man man mit der Maus (Drag&Drop zur rotation, strg+ drag&drop zur translation) an die gewünschten stellen der meshes. 
abwechselnd fährt man nun über das linke mesh, drückt [l]  (kleines L), ein triangle wird markiert, wobei das selektierte Vertex farbig erscheint. 
Möchte man ein anderes Vertex desselben dreiecks auswählen, drückt man erneut [l]. Für das Rechte vertex ist die Taste zum Auswählen [r]. 
Auch hier kann man durch mehrfaches Drücken ein bestimmtes Vertex eines Dreiecks auswählen. 

Nachdem abwechselnd Links und Rechts dreiecke ausgewählt wurden (Reihenfolge ist wichtig!) wird durch das Drücken von [s] eine Datei mit den beinhaltenden Vertexindices gespeichert.
(correspondences.csv).


Danach sollte man für das ziel Mesh die Adjazenz berechnung mittels des AdjCalc durchführen. 
(Dieser Schritt dauert sehr lange, da bisher nur ein bruteforce ansatz implementiert ist. Da dies jedoch offline geschehen kann, habe ich hier keine optimierung vorgenommen).


Der Corressolver versucht nun das targetMesh an das SourceMesh anzupassen und danach ein Mapping aller Triangles herzustellen, 
so dass zu jedem targetTriangle ein Sourcetriangle gemappt ist.

Das Mapping wird in der resolved_correspondences.csv gespeicher. 

Mit dem Mapping, dem Sourcereference mesh, target reference mesh und dem source deformed mesh lässt sich dann mittels defTrans das Target mesh so verformen, wie das sourceref zu source deformed verformt wurde.