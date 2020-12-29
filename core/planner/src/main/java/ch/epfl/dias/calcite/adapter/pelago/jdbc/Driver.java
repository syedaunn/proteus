package ch.epfl.dias.calcite.adapter.pelago.jdbc;

import org.apache.calcite.avatica.*;
import org.apache.calcite.jdbc.CalcitePrepare;
import org.apache.calcite.jdbc.PelagoMetaImpl;
import org.apache.calcite.linq4j.function.Function0;
import org.apache.calcite.prepare.PelagoPrepareImpl;

import java.sql.Connection;
import java.sql.SQLException;

public class Driver extends org.apache.calcite.jdbc.Driver {
    public static final String CONNECT_STRING_PREFIX = "jdbc:pelago:";

    static {
        new Driver().register();
    }

    @Override protected String getConnectStringPrefix() {
        return CONNECT_STRING_PREFIX;
    }

    protected DriverVersion createDriverVersion() {
        return DriverVersion.load(
                Driver.class,
                "ch-epfl-dias-pelago-jdbc.properties",
                "Pelago JDBC Driver",
                "unknown version",
                "Pelago",
                "unknown version");
    }

    protected Function0<CalcitePrepare> createPrepareFactory() {
        return new Function0<CalcitePrepare>() {
            public CalcitePrepare apply() {
                return new PelagoPrepareImpl();
            }
        };
    }

    @Override public Meta createMeta(AvaticaConnection connection) {
        return new PelagoMetaImpl(connection);
    }

    public Connection connect(String url, java.util.Properties info) throws SQLException{
        info.put("lex", "JAVA");//Lex.JAVA);
        info.put("parserFactory", "ch.epfl.dias.calcite.sql.parser.ddl.SqlDdlParserImpl#FACTORY");
        return super.connect(url, info);
    }
}
