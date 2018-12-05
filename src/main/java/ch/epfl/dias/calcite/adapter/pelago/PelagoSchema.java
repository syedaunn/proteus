package ch.epfl.dias.calcite.adapter.pelago;

import ch.epfl.dias.calcite.adapter.pelago.reporting.TimeKeeperTable;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.calcite.config.Lex;
import org.apache.calcite.schema.SchemaFactory;
import org.apache.calcite.schema.Table;
import org.apache.calcite.schema.impl.AbstractSchema;
import org.apache.calcite.util.Source;
import org.apache.calcite.util.Sources;

import com.google.common.collect.ImmutableMap;
import org.xml.sax.ext.LexicalHandler;

import java.io.File;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.Map;

/**
 * Schema from a Pelago catalog.
 */
public class PelagoSchema extends AbstractSchema {
  private final File directoryFile;
  private Map<String, Table> tableMap;

  /**
   * Creates a Pelago schema.
   *
   * @param directoryFile Directory that holds catalog.json file
   */
  public PelagoSchema(File directoryFile) {
    super();
    assert(directoryFile != null);
    this.directoryFile = directoryFile;
  }

  @Override protected Map<String, Table> getTableMap() {
    if (tableMap == null) tableMap = createTableMap();

    return tableMap;
  }

  private void parseCatalogFile(File file, ImmutableMap.Builder<String, Table> builder){
    ObjectMapper mapper = new ObjectMapper();
    Map<String, ?> catalog;
    try {
      catalog = mapper.readValue(file, new TypeReference<Map<String, ?>>() {});
    } catch (java.io.IOException e){
      System.err.println("Catalog not found " + directoryFile + " not found");
      return;
    }

    for (Map.Entry<String, ?> e: catalog.entrySet()) {
//      System.out.println("Table Found: " + e.getKey());
//      System.out.println("   Row Type: " + ((Map<String, ?>) e.getValue()).get("type").toString());
      Map<String, ?> fileEntry = (Map<String, ?>) ((Map<String, ?>) e.getValue()).get("type");
      String fileType = (String) fileEntry.getOrDefault("type", null);
      if (!fileType.equals("bag")) {
        System.err.println("Error in catalog: relation type is expected to be \"bag\", but \"" + fileType + "\" found");
        System.out.println("Ignoring table: " + e.getKey());
        continue;
      }
      Map<String, ?> lineType = (Map<String, ?>) fileEntry.getOrDefault("inner", null);
      if (lineType != null && !lineType.getOrDefault("type", null).equals("record")) lineType = null;
      if (lineType == null) {
        System.err.println("Error in catalog: \"bag\" expected to contain records");
        System.out.println("Ignoring table: " + e.getKey());
        continue;
      }
      Source source = Sources.of(new File((String) ((Map<String, ?>) e.getValue()).get("path")));

      Map<String, ?> plugin = (Map<String, ?>) ((Map<String, ?>) e.getValue()).getOrDefault("plugin", null);
      if (plugin == null) {
        System.err.println("Error in catalog: plugin information not found for table");
        System.out.println("Ignoring table: " + e.getKey());
        continue;
      }

      try {
        final Table table = PelagoTable.create(source, e.getKey(), plugin, lineType);
        System.out.println("Table: " + e.getKey());
        builder.put(e.getKey(), table); //.toUpperCase(Locale.getDefault())
      } catch (MalformedPlugin malformedPlugin) {
        System.out.println("Error in catalog: " + malformedPlugin.getMessage  ());
        System.out.println("Ignoring table  : " + malformedPlugin.getTableName());
        continue;
      }
    }
  }

  private void recursiveParse(File f, ImmutableMap.Builder<String, Table> builder){
    for(File file: f.listFiles()){
      if (file.getName() == "..") continue;
      if (file.getName() == "." ) continue;

      try {
        if (file.toPath().toRealPath().toFile().isDirectory()){
          recursiveParse(file, builder);
        } else if ("catalog.json".equals(file.getName()) && file.isFile()){
          parseCatalogFile(file, builder);
        } /* else skip */
      } catch (IOException e) {
        /* ignore */
      }
    }
  }

  private Map<String, Table> createTableMap() {
    //FIXME: long term, it would be better to ask the executor for the list of files, __or__ make executor agnostic,
    // we are already passing all the necessary information, so does the executor actually needs a copy of the catalog?
    // except from backwards compatibility?

    // Build a map from table name to table; each file becomes a table.
    final ImmutableMap.Builder<String, Table> builder = ImmutableMap.builder();

    recursiveParse(directoryFile, builder);

    // FIXME adding the time keeper table
    builder.put("SessionTimings", TimeKeeperTable.INSTANCE);

    return builder.build();
  }
}

// End PelagoSchema.java


