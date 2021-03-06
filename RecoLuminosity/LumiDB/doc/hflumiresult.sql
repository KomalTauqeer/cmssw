CREATE TABLE "HFLUMIRESULT"
(	"RUNNUM" NUMBER(10,0) NOT NULL,
	"LS" NUMBER(10,0) NOT NULL ,
	"CMSLS" NUMBER(10,0) NOT NULL ,
	"FILLNUM" NUMBER(10,0) NOT NULL ,
	"TIME" TIMESTAMP(6) NOT NULL ,
	"BEAM_STATUS" VARCHAR2(50) NOT NULL,
	"ENERGY" NUMBER(10,0) NOT NULL,
     	"DELIVERED" BINARY_FLOAT, 
        "RECORDED" BINARY_FLOAT,
        "AVG_PU" BINARY_FLOAT,
        "DATA_VERSION" VARCHAR2(50) NOT NULL ,
	"NORM_VERSION" VARCHAR2(50) NOT NULL ,
        "INSERT_TIME" TIMESTAMP NOT NULL ,	
	CONSTRAINT "HFLUMIRESULT_PK" PRIMARY KEY ("RUNNUM","LS")
)
PARTITION BY RANGE(RUNNUM)
(
PARTITION YEAR2010 VALUES LESS THAN(153368),
PARTITION YEAR2011 VALUES LESS THAN(183338),
PARTITION YEAR2012 VALUES LESS THAN(209310),
PARTITION THEREST VALUES LESS THAN(MAXVALUE)
)
;
GRANT SELECT ON "HFLUMIRESULT" TO PUBLIC;
GRANT SELECT,INSERT,DELETE,UPDATE ON "HFLUMIRESULT" TO CMS_LUMI_WRITER;
CREATE INDEX HFLUMIRESULT_RUNNUM_IDX ON HFLUMIRESULT(RUNNUM) LOCAL;


