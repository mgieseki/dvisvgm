<?xml version="1.0"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2020 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:date="http://exslt.org/dates-and-times"
	xmlns:func="http://exslt.org/functions"
	xmlns:my="my-namespace"
	exclude-result-prefixes="my"
	extension-element-prefixes="date func">

	<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN" doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

	<!-- adapt ISO date created by keyword expansion -->
	<xsl:template match="refentryinfo/date | revision[1]/date">
		<date>
			<xsl:value-of select="my:extract-date(.)"/>
		</date>
	</xsl:template>

	<!-- add date to refmeta element -->
	<xsl:template match="refmeta">
		<xsl:copy>
			<xsl:copy-of select="*"/>
			<refmiscinfo class="date">
				<xsl:value-of select="my:extract-date(../refentryinfo/date)"/>
			</refmiscinfo>
		</xsl:copy>
	</xsl:template>

	<func:function name="my:extract-date">
		<xsl:param name="isodate"/>
		<xsl:variable name="shortdate" select="substring($isodate, 1, 10)"/>
		<xsl:choose>
			<xsl:when test="translate($shortdate, '0123456789', '##########') = '####-##-##'">
				<!-- use date part of expanded keyword -->
				<func:result select="$shortdate"/>
			</xsl:when>
			<xsl:otherwise>
				<!-- use current date as fallback -->
				<func:result select="substring(date:date(), 1, 10)"/>
			</xsl:otherwise>
		</xsl:choose>
	</func:function>

	<!-- copy everything else unchanged -->
	<xsl:template match="@*|node()">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
