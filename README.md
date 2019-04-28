# deformationTransfer

## Testing:

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
(Dieser Schritt dauert sehr lange, da bisher nur ein bruteforce ansatz implementiert ist. Da dies jedoch offline geschehen kann und nur einmalig für ein targetMesh durchgeführt werden muss, habe ich hier keine optimierung außer parallelisierung vorgenommen).


Der Corressolver versucht nun das targetMesh an das SourceMesh anzupassen und danach ein Mapping aller Triangles herzustellen, 
so dass zu jedem targetTriangle ein Sourcetriangle gemappt ist.

Das Mapping wird in der resolved_correspondences.csv gespeicher. 

Mit dem Mapping, dem Sourcereference mesh, target reference mesh und dem source deformed mesh lässt sich dann mittels defTrans das Target mesh so verformen, wie das sourceref zu source deformed verformt wurde.

Das deformierte Mesh wird dann als deformed.obj abgespeichert. 


In Ordner Testing befindet sich ebenfalls eine Python datei. 
Mit dem Paket trimesh kann man damit schnell .obj Dateien ansehen. 
z.B. mit `python3 show_ObjFile.py deformed.obj`

## Dependencies

* UMFPACK (suitesparse) - in external enthalten
* CHOLMOD (suitesparse) - in external enthalten
* eigen3                - nicht enthalten aber über `sudo apt-get install libeigen-dev` installierbar
* pointcloudlib         - nicht enthalten aber über `sudo apt-get install libpcl-dev` installierbar
* openGL und GLUT       - nicht enthalten aber über `sudo apt-get install mesa-utils free-glut3-dev` installierbar
* OpenMP                - nicht enthalten aber über `sudo apt-get install libomp-dev` installierbar

   (falls mit CHOLMOD statt mit UMFPACK die least squares probleme gelöst werden)
 




