<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="mskversion" select="$mskversion"/>
  <xsl:output omit-xml-declaration="yes" indent="yes"/>

  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
 </xsl:template>

  <xsl:template match="Updates/Checksum">
    <Checksum>true</Checksum>
    <RepositoryUpdate>
      <Repository action="add" url="../{$mskversion}" displayname="Molsketch {$mskversion} Update"/>
    </RepositoryUpdate>
  </xsl:template>
</xsl:stylesheet>
