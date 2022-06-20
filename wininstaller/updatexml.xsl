<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="mskversion" select="$mskversion"/>

  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
 </xsl:template>

  <xsl:template match="Updates/ApplicationName">
    <ApplicationName>Molsketch</ApplicationName>
  </xsl:template>

  <xsl:template match="Updates/ApplicationVersion">
    <ApplicationVersion><xsl:value-of select="$mskversion"/></ApplicationVersion>
  </xsl:template>

  <xsl:template match="Updates/PackageUpdate/Version[preceding-sibling::Name[.='org.molsketch']]">
    <Version><xsl:value-of select="$mskversion"/></Version>
  </xsl:template>

  <xsl:template match="Updates/PackageUpdate/Version[preceding-sibling::Name[.='org.molsketch.openbabel.adapter']]">
    <Version><xsl:value-of select="$mskversion"/></Version>
  </xsl:template>

</xsl:stylesheet>
