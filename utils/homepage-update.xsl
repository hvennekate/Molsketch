<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="ul[@id='version-history']">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
        <xsl:text>
          </xsl:text>
        <li><xsl:value-of select="$date"/></li>
        <xsl:text>
          </xsl:text>
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="$blogpostUrl"/>
            </xsl:attribute>
            <xsl:value-of select="$mskversionnick"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="$mskversion"/>
          </a>
        </li>
      <xsl:copy-of select="node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
