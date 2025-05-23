PULSONIX_LIBRARY_ASCII "SamacSys ECAD Model"
//467776/1547573/2.50/2/3/Capacitor Polarised

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r290_180"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 1.8) (shapeHeight 2.9))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Normal"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 1.27)
			(strokeWidth 0.127)
		)
	)
	(patternDef "CAPAE530X550N" (originalName "CAPAE530X550N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r290_180) (pt -2, 0) (rotation 90))
			(pad (padNum 2) (padStyleRef r290_180) (pt 2, 0) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Normal") (isVisible True))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -3.95 3.25) (pt 3.95 3.25) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 3.95 3.25) (pt 3.95 -3.25) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 3.95 -3.25) (pt -3.95 -3.25) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -3.95 -3.25) (pt -3.95 3.25) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.65 2.65) (pt -1.325 2.65) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.325 2.65) (pt -2.65 1.325) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.65 1.325) (pt -2.65 -1.325) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.65 -1.325) (pt -1.325 -2.65) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.325 -2.65) (pt 2.65 -2.65) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.65 -2.65) (pt 2.65 2.65) (width 0.025))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.45 2.65) (pt 2.65 2.65) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.325 -2.65) (pt 2.65 -2.65) (width 0.2))
		)
	)
	(symbolDef "865230542002" (originalName "865230542002")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 0 mils -35 mils) (rotation 0]) (justify "UpperLeft") (textStyleRef "Normal"))
		))
		(pin (pinNum 2) (pt 500 mils 0 mils) (rotation 180) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 500 mils -35 mils) (rotation 0]) (justify "UpperRight") (textStyleRef "Normal"))
		))
		(line (pt 200 mils 100 mils) (pt 200 mils -100 mils) (width 6 mils))
		(line (pt 200 mils -100 mils) (pt 230 mils -100 mils) (width 6 mils))
		(line (pt 230 mils -100 mils) (pt 230 mils 100 mils) (width 6 mils))
		(line (pt 230 mils 100 mils) (pt 200 mils 100 mils) (width 6 mils))
		(line (pt 180 mils 50 mils) (pt 140 mils 50 mils) (width 6 mils))
		(line (pt 160 mils 70 mils) (pt 160 mils 30 mils) (width 6 mils))
		(line (pt 100 mils 0 mils) (pt 200 mils 0 mils) (width 6 mils))
		(line (pt 300 mils 0 mils) (pt 400 mils 0 mils) (width 6 mils))
		(poly (pt 300 mils 100 mils) (pt 300 mils -100 mils) (pt 270 mils -100 mils) (pt 270 mils 100 mils) (pt 300 mils 100 mils) (width 10  mils))
		(attr "RefDes" "RefDes" (pt 350 mils 250 mils) (justify 24) (isVisible True) (textStyleRef "Normal"))
		(attr "Type" "Type" (pt 350 mils 150 mils) (justify 24) (isVisible True) (textStyleRef "Normal"))

	)
	(compDef "865230542002" (originalName "865230542002") (compHeader (numPins 2) (numParts 1) (refDesPrefix C)
		)
		(compPin "1" (pinName "+") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "2" (pinName "-") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Unknown))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "865230542002"))
		(attachedPattern (patternNum 1) (patternName "CAPAE530X550N")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Mouser Part Number" "710-865230542002")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Wurth-Elektronik/865230542002?qs=0KOYDY2FL2%2F9Zrx1rvGWxQ%3D%3D")
		(attr "Manufacturer_Name" "Wurth Elektronik")
		(attr "Manufacturer_Part_Number" "865230542002")
		(attr "Description" "Wurth Elektronik 10uF 35 V dc Aluminium Electrolytic Capacitor, WCAP-AS5H Series 5000h 5.5 (Dia.) x 4.85mm")
		(attr "<Hyperlink>" "http://katalog.we-online.com/pbs/datasheet/865230542002.pdf")
		(attr "<Component Height>" "5.5")
		(attr "<STEP Filename>" "865230542002.stp")
		(attr "<STEP Offsets>" "X=0;Y=0;Z=0")
		(attr "<STEP Rotation>" "X=0;Y=0;Z=0")
	)

)
